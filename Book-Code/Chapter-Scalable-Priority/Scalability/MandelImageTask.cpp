/*
Copyright (c) 2015 James Dean Mathias

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include "MandelImageTask.hpp"

#include "IRange.hpp"
#include "MandelPartTask.hpp"
#include "ThreadPool.hpp"

// ------------------------------------------------------------------
//
// @details Constructor that takes the Mandelbrot computation parameters
//
// ------------------------------------------------------------------
MandelImageTask::MandelImageTask(double startX, double endX, double startY, double endY, uint16_t maxIterations, uint16_t sizeX, uint16_t sizeY, uint8_t* pixels, uint16_t stride, std::function<void ()> onComplete, Priority priority) :
	Task(onComplete, priority),
	m_startX(startX),
	m_endX(endX),
	m_startY(startY),
	m_endY(endY),
	m_maxIterations(maxIterations),
	m_sizeX(sizeX),
	m_sizeY(sizeY),
	m_pixels(pixels),
	m_stride(stride),
	m_partsFinished(0),
	m_image(nullptr)
{
	//
	// Allocate memory for the image
	m_image = std::unique_ptr<uint16_t[]>(new uint16_t[sizeX * sizeY]);
}

// ------------------------------------------------------------------
//
// @details This is where the image and task delegation work is performed.
//
// ------------------------------------------------------------------
void MandelImageTask::execute()
{
	//
	// For each row, compute how far to move in each of the y direction
	double deltaY = (m_endY - m_startY) / m_sizeY;

	//
	// Create a bunch of smaller tasks that compute individual rows in the image
	for (auto row : IRange<decltype(m_sizeY)>(0, m_sizeY - 1))
	{
		auto task = std::make_shared<MandelPartTask>(
			m_image.get() + row * m_sizeX, 
			m_sizeX, m_maxIterations,
			m_startY + row * deltaY,
			m_startX, m_endX,
			std::bind(&MandelImageTask::completePart, this),
			this->getPriority());
		ThreadPool::instance()->enqueueTask(task);
	}

	prepareColors();

	//
	// Wait for all of these items to finish computing
	std::unique_lock<std::mutex> lock(m_mutexImageFinished);
	m_imageFinished.wait(lock,
		[this]()
		{
			//
			// Might be a spurious wakeup, make sure we have really received all the parts.
			return m_partsFinished == m_sizeY;
		});

	copyToPixels();
}

// ------------------------------------------------------------------
//
// @details Builds the color (gradient) array used for smooth coloring the Mandelbrot
//
// ------------------------------------------------------------------
void MandelImageTask::prepareColors()
{
		for (auto position : IRange<uint16_t>(0, 767))
		{
			m_colors[position].r = 0;
			m_colors[position].g = 0;
			m_colors[position].b = 0;
			if (position >= 512)
			{
				m_colors[position].r = position - 512;
				m_colors[position].g = 255 - m_colors[position].r;
			}
			else if (position >= 256)
			{
				m_colors[position].g = position - 256;
				m_colors[position].b = 255 - m_colors[position].g;
			}
			else
			{
				m_colors[position].b = static_cast<uint8_t>(position);
			}
		}
}

// ------------------------------------------------------------------
//
// @details This is used to copy the raw mandel computed data into
// the bitmap used for rendering.  Nothing fancy here, just a simple
// grey-scale image.
//
// ------------------------------------------------------------------
void MandelImageTask::copyToPixels()
{
	//
	// Doing a little optimization through the use of pointers to copy
	// the data into the destination bitmap.
	uint16_t* source = m_image.get();
	uint8_t* destination = m_pixels;
	for (auto row : IRange<decltype(m_sizeY)>(0, m_sizeY - 1))
	{
		uint8_t* destination = m_pixels + row * m_stride;
		for (auto column : IRange<decltype(m_sizeX)>(0, m_sizeX - 1))
		{
			uint16_t color = *(source++);
			*(destination++) = m_colors[color].r;
			*(destination++) = m_colors[color].g;
			*(destination++) = m_colors[color].b;
			*(destination++) = 0;
		}
	}
}

// ------------------------------------------------------------------
//
// @details This is the handler used to allow a sub-task to notify
// this task that it has finished with its part of the image computation.
//
// ------------------------------------------------------------------
void MandelImageTask::completePart()
{
	m_partsFinished++;
	if (m_partsFinished == m_sizeY)
	{
		m_imageFinished.notify_one();
	}
}
