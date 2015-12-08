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

#include "MandelPartTask.hpp"

#include "IRange.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <thread>

namespace
{
	// -----------------------------------------------------------------
	//
	// @details This method computes the value for a single point in the image
	//
	// -----------------------------------------------------------------
	uint16_t computePoint(double x0, double y0, uint16_t maxIterations)
	{
		double x = 0;
		double y = 0;

		//
		// Declaring distance outsize the loop because it is needed by the smooth
		// coloring algorithm.
		double distance = 0;
		uint16_t iterations = 0;
		bool done = false;

		while (!done)
		{
			//
			// Let's do blocks of iterations to improve performance, only testing
			// ever so often rather than every single pixel.
			for (uint8_t block = 0; block < 10; block++)
			{
				double tempX = x * x - y * y + x0;
				y = 2.0 * x * y + y0;

				x = tempX;

				distance = x * x + y * y;
				if (distance > 4.0 || iterations >= maxIterations)
				{
					done = true;
				}
				iterations++;
			}
		}

		return iterations;
	}
}

// ------------------------------------------------------------------
//
// @details Constructor
//
// ------------------------------------------------------------------
MandelPartTask::MandelPartTask(uint16_t* imageRow, uint16_t resolution, uint16_t maxIterations, double y, double startX, double endX, std::function<void ()> onComplete, Priority priority) :
	Task(onComplete, priority),
	m_imageRow(imageRow),
	m_resolution(resolution),
	m_maxIterations(maxIterations),
	m_y(y),
	m_startX(startX),
	m_endX(endX)
{
}

// ------------------------------------------------------------------
//
// @details This method manages the computation of a single row in the image.
//
// ------------------------------------------------------------------
void MandelPartTask::execute()
{
	//
	// Compute this only one time for all pixels.
	auto log2MaxIterations = std::log2(std::log(m_maxIterations));

	double deltaX = (m_endX - m_startX) / m_resolution;
	for (auto x : IRange<decltype(m_resolution)>(0, m_resolution - 1))
	{
		auto iterations = computePoint(m_startX + x * deltaX, m_y, m_maxIterations);

		//
		// Use the smooth coloring algorithm to determine the color index;
		double colorIndex = iterations - log2MaxIterations;
		colorIndex = (colorIndex / m_maxIterations) * 768;
		colorIndex = std::min(colorIndex, 767.0);
		colorIndex = std::max(colorIndex, 0.0);

		m_imageRow[x] = static_cast<uint16_t>(colorIndex);
	}
}
