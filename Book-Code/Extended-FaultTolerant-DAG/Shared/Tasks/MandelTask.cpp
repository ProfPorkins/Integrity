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

#include "MandelTask.hpp"

#include "Shared/IRange.hpp"
#include "Shared/Messages/MandelMessage.hpp"
#include "Shared/Messages/MandelResult.hpp"

#include <algorithm>
#include <cmath>
#include <memory>

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

namespace Tasks
{
	// -----------------------------------------------------------------
	//
	// @details This manages the computation of the pixels for which this
	// task is responsible.
	//
	// -----------------------------------------------------------------
	void MandelTask::execute()
	{
		//
		// Compute this only one time for all pixels.
		auto log2MaxIterations = std::log2(std::log(m_maxIterations));

		//
		// Now that we are about to do some work, reserve the memory we need to store the results.
		uint16_t rows = (m_endRow - m_startRow) + 1;
		m_pixels.resize(rows * m_sizeX);

		//
		// Premature optimization, I know, but just can't help myself.
		auto pixels = m_pixels.data();

		double currentY = m_startY;
		for (int row = 0; row < rows; row++, currentY += m_deltaY)
		{
			double currentX = m_startX;
			for (int x = 0; x < m_sizeX; x++, currentX += m_deltaX)
			{
				auto iterations = computePoint(currentX, currentY, m_maxIterations);

				//
				// Use the smooth coloring algorithm to determine the color index;
				double colorIndex = iterations - log2MaxIterations;
				colorIndex = (colorIndex / m_maxIterations) * 768;
				colorIndex = std::min(colorIndex, 767.0);
				colorIndex = std::max(colorIndex, 0.0);

				pixels[row * m_sizeX + x] = static_cast<uint16_t>(colorIndex);
			}
		}
	}

	// -----------------------------------------------------------------
	//
	// @details Builds the message used to indicate what work is to be done
	//
	// -----------------------------------------------------------------
	std::shared_ptr<Messages::Message> MandelTask::getMessage()
	{
		return std::shared_ptr<Messages::MandelMessage>(new Messages::MandelMessage(m_id, m_startRow, m_endRow, m_sizeX, m_startX, m_startY, m_deltaX, m_deltaY, m_maxIterations));
	}

	// ------------------------------------------------------------------
	//
	// @details Prepare the result message to send back to the client
	//
	// ------------------------------------------------------------------
	std::shared_ptr<Messages::Message> MandelTask::completeCustom(boost::asio::io_service& ioService)
	{
		//
		// Generate the message and return it to the calling code
		std::shared_ptr<Messages::MandelResult> message = std::make_shared<Messages::MandelResult>(this->getId(), m_startRow, m_endRow, m_pixels);
		return message;
	}
}
