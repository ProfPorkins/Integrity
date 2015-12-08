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

#ifndef _MANDELTASK_HPP_
#define _MANDELTASK_HPP_

#include "Shared/Messages/MandelMessage.hpp"
#include "Task.hpp"

#include <memory>
#include <vector>

namespace Tasks
{
	// ------------------------------------------------------------------
	//
	// @details This class represents a sub-image Mandelbrot work unit.
	// This class computes and returns the results for a specified
	// region of the Mandelbrot image.
	//
	// ------------------------------------------------------------------
	class MandelTask : public Task
	{
	public:
		MandelTask(uint16_t startRow, uint16_t endRow, uint16_t sizeX, double startX, double startY, double deltaX, double deltaY, uint16_t maxIterations, Priority priority = Priority::One) :
			Task(std::chrono::milliseconds(1000), priority),
			m_startRow(startRow),
			m_endRow(endRow),
			m_sizeX(sizeX),
			m_startX(startX),
			m_startY(startY),
			m_deltaX(deltaX),
			m_deltaY(deltaY),
			m_maxIterations(maxIterations)
		{
		}

		MandelTask(std::shared_ptr<ip::tcp::socket> socket, Messages::MandelMessage& message) :
			Task(socket, message.getTaskId()),
			m_startRow(message.m_startRow),
			m_endRow(message.m_endRow),
			m_sizeX(message.m_sizeX),
			m_startX(message.m_startX),
			m_startY(message.m_startY),
			m_deltaX(message.m_deltaX),
			m_deltaY(message.m_deltaY),
			m_maxIterations(message.m_maxIterations)
		{
		}

		virtual void execute() override;

	protected:
		virtual std::shared_ptr<Messages::Message> getMessage() override;
		virtual std::shared_ptr<Messages::Message> completeCustom(boost::asio::io_service& ioService) override;

	private:
		uint16_t m_startRow;
		uint16_t m_endRow;
		uint16_t m_sizeX;
		double m_startX;
		double m_startY;
		double m_deltaX;
		double m_deltaY;
		uint16_t m_maxIterations;
		std::vector<uint16_t> m_pixels;
	};
}

#endif // _MANDELTASK_HPP_
