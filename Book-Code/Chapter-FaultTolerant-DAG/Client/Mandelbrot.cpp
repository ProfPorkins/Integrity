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

#include "Mandelbrot.hpp"

#include "Shared/IRange.hpp"
#include "Shared/Tasks/MandelFinishedTask.hpp"
#include "Shared/Tasks/MandelTask.hpp"
#include "TaskRequestQueue.hpp"

// -----------------------------------------------------------------
//
// @details Initialize the Mandelbrot region, the mandelbrot rendering
// bitmap, and kick off the generation of prime numbers.
//
// -----------------------------------------------------------------
Mandelbrot::Mandelbrot(HWND hwnd, uint16_t sizeX, uint16_t sizeY) :
	m_hwnd(hwnd),
	m_sizeX(sizeX),
	m_sizeY(sizeY),
	m_updateRequired(true),
	m_inUpdate(false),
	m_mandelLeft(-2.0),
	m_mandelRight(1.0),
	m_mandelTop(-1.5),
	m_mandelBottom(1.5)
{
	prepareColors();
	prepareBitmap();
	//
	// Allocate memory for the intermediate image
	m_image = std::unique_ptr<uint16_t[]>(new uint16_t[sizeX * sizeY]);
}

// -----------------------------------------------------------------
//
// @details Called by the main application event loop to allow this class
// to perform any rendering or image computation tasks, as needed.
//
// -----------------------------------------------------------------
void Mandelbrot::update()
{
	if (m_updateRequired && !m_inUpdate)
	{
		m_inUpdate = true;
		m_updateRequired = false;
		startNewImage();
	}

	//
	// Render the current mandelbrot set
	HDC hdc = GetDC(m_hwnd);
	SelectObject(m_hdcBMP, m_handleBMPWrite);
	BitBlt(hdc, 0, 0, m_sizeX - 1, m_sizeY - 1, m_hdcBMP, 0, 0, SRCCOPY);
	ReleaseDC(m_hwnd, hdc);
}

// -----------------------------------------------------------------
//
// @details Copies the results into the current image and checks to see
// if we are still waiting for additional results to arrive before
// allowing the next image to be computed.
//
// -----------------------------------------------------------------
void Mandelbrot::processMandelResult(Messages::MandelResultMessage& taskResult)
{
	//
	// Copy these pixels into our displayable image
	copyPixels(taskResult);
}

// -----------------------------------------------------------------
//
// @details Indiate we are no longer waiting for an outstanding Mandelbrot
// computation to complete.
//
// -----------------------------------------------------------------
void Mandelbrot::processMandelFinishedResult(Messages::MandelFinishedResultMessage& taskResult)
{
	m_inUpdate = false;
}

// -----------------------------------------------------------------
//
// @details This method generates all the sub-image tasks that perform the
// actual computational work.
//
// -----------------------------------------------------------------
void Mandelbrot::startNewImage()
{
	TaskRequestQueue::instance()->beginGroup();

	//
	// Create the task that is executed when all of the small sub-image tasks
	// have completed...the framework DAG takes care of this for us automatically!
	//
	// NOTE: This task never directly gets added to the queue.  Because it is identified
	// as a dependent task by other tasks, it gets added at that time.
	auto taskFinished = std::make_shared<Tasks::MandelFinishedTask>();

	double deltaX = (m_mandelRight - m_mandelLeft) / m_sizeX;
	double deltaY = (m_mandelBottom - m_mandelTop) / m_sizeY;
	
	for (auto row : IRange<uint16_t>(0, m_sizeY - 1, MANDEL_COMPUTE_ROWS))
	{
		auto task = std::make_shared<Tasks::MandelTask>(
			row, std::min(row + MANDEL_COMPUTE_ROWS - 1, m_sizeY - 1), 
			m_sizeX, m_mandelLeft, m_mandelTop + row * deltaY, 
			deltaX, deltaY, 
			MANDLE_MAX_ITERATIONS);
		TaskRequestQueue::instance()->enqueueTask(task, taskFinished);
	}

	TaskRequestQueue::instance()->endGroup();
}

// -----------------------------------------------------------------
//
// @details Builds the color (gradient) array used for smooth coloring the Mandelbrot
//
// -----------------------------------------------------------------
void Mandelbrot::prepareColors()
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

// -----------------------------------------------------------------
//
// @details Moves the Mandelbrot view left
//
// -----------------------------------------------------------------
void Mandelbrot::moveLeft()
{ 
	if (!m_inUpdate)
	{
		double distance = (m_mandelRight - m_mandelLeft) * MANDEL_MOVEMENT_RATE;
		m_mandelLeft -= distance;
		m_mandelRight -= distance;
		m_updateRequired = true;
	}
}

// -----------------------------------------------------------------
//
// @details Moves the Mandelbrot view right
//
// -----------------------------------------------------------------
void Mandelbrot::moveRight()
{
	if (!m_inUpdate)
	{
		double distance = (m_mandelRight - m_mandelLeft) * MANDEL_MOVEMENT_RATE;
		m_mandelLeft += distance;
		m_mandelRight += distance;
		m_updateRequired = true;
	}
}

// -----------------------------------------------------------------
//
// @details Moves the Mandelbrot view up
//
// -----------------------------------------------------------------
void Mandelbrot::moveUp()
{
	if (!m_inUpdate)
	{
		double distance = (m_mandelBottom - m_mandelTop) * MANDEL_MOVEMENT_RATE;
		m_mandelTop += distance;
		m_mandelBottom += distance;
		m_updateRequired = true;
	}
}

// -----------------------------------------------------------------
//
// @details Moves the Mandelbrot view down
//
// -----------------------------------------------------------------
void Mandelbrot::moveDown()
{
	if (!m_inUpdate)
	{
		double distance = (m_mandelBottom - m_mandelTop) * MANDEL_MOVEMENT_RATE;
		m_mandelTop -= distance;
		m_mandelBottom -= distance;
		m_updateRequired = true;
	}
}

// -----------------------------------------------------------------
//
// @details Moves the Mandelbrot view in
//
// -----------------------------------------------------------------
void Mandelbrot::zoomIn()
{
	if (!m_inUpdate)
	{
		double distanceX = (m_mandelRight - m_mandelLeft) * MANDEL_MOVEMENT_RATE;
		double distanceY = (m_mandelBottom - m_mandelTop) * MANDEL_MOVEMENT_RATE;

		m_mandelLeft += distanceX;
		m_mandelRight -= distanceX;

		m_mandelTop += distanceY;
		m_mandelBottom -= distanceY;

		m_updateRequired = true;
	}
}

// -----------------------------------------------------------------
//
// @details Moves the Mandlebrot view out
//
// -----------------------------------------------------------------
void Mandelbrot::zoomOut()
{
	if (!m_inUpdate)
	{
		double distanceX = (m_mandelRight - m_mandelLeft) * MANDEL_MOVEMENT_RATE;
		double distanceY = (m_mandelBottom - m_mandelTop) * MANDEL_MOVEMENT_RATE;

		m_mandelLeft -= distanceX;
		m_mandelRight += distanceX;

		m_mandelTop -= distanceY;
		m_mandelBottom += distanceY;

		m_updateRequired = true;
	}
}

// -----------------------------------------------------------------
//
// @details This builds a bitmap that is compatible with the current
// display, according to the size requested by the application.
//
// -----------------------------------------------------------------
void Mandelbrot::prepareBitmap()
{
	HDC hdc = GetDC(m_hwnd);
	m_handleBMP = CreateCompatibleBitmap(hdc, m_sizeX, m_sizeY);
	ReleaseDC(m_hwnd, hdc);

	GetObject(m_handleBMP, sizeof(BITMAP), &m_bmp);
	m_hdcBMP = CreateCompatibleDC(NULL);

	BITMAPINFO bmiMandel;
	bmiMandel.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmiMandel.bmiHeader.biWidth = m_sizeX;
	bmiMandel.bmiHeader.biHeight = m_sizeY;
	bmiMandel.bmiHeader.biPlanes = 1;
	bmiMandel.bmiHeader.biBitCount = m_bmp.bmBitsPixel;
	bmiMandel.bmiHeader.biCompression = BI_RGB;
	bmiMandel.bmiHeader.biSizeImage = 0;
	bmiMandel.bmiHeader.biXPelsPerMeter = 0;
	bmiMandel.bmiHeader.biYPelsPerMeter = 0;
	bmiMandel.bmiHeader.biClrUsed = 0;
	bmiMandel.bmiHeader.biClrImportant = 0;

	m_handleBMPWrite = CreateDIBSection(
		m_hdcBMP, &bmiMandel,
		DIB_RGB_COLORS,
		reinterpret_cast<void **>(&m_pixels), NULL, 0);
}

// -----------------------------------------------------------------
//
// @details This method takes the results from a distributed mandel
// sub-image computation and copies them into the displayable image.
//
// -----------------------------------------------------------------
void Mandelbrot::copyPixels(Messages::MandelResultMessage& taskResult)
{
	auto source = taskResult.getPixels().data();
	for (uint16_t row = taskResult.getStartRow(); row <= taskResult.getEndRow(); row++)
	{
		uint8_t* destination = m_pixels + row * m_bmp.bmWidthBytes;
		for (uint16_t x = 0; x < m_sizeX; x++)
		{
			uint16_t color = (*source++);
			*(destination++) = m_colors[color].r;
			*(destination++) = m_colors[color].g;
			*(destination++) = m_colors[color].b;
			*(destination++) = 0;
		}
	}
}
