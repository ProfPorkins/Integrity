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

#ifndef _MANDELMESSAGE_HPP_
#define _MANDELMESSAGE_HPP_

#include "TaskMessage.hpp"

namespace Tasks
{
	class MandelTask;
}

namespace Messages
{
	// -----------------------------------------------------------------
	//
	// @details This class is used to send a mandel task message to 
	// a compute node.
	//
	// -----------------------------------------------------------------
	class MandelMessage : public TaskMessage
	{
	public:
		MandelMessage() :
			TaskMessage(Messages::Type::MandelMessage)
		{
		}

		MandelMessage(uint32_t taskId, uint16_t startRow, uint16_t endRow, uint16_t sizeX, double startX, double startY, double deltaX, double deltaY, uint16_t maxIterations) :
			TaskMessage(Messages::Type::MandelMessage, taskId),
			m_startRow(startRow),
			m_endRow(endRow),
			m_startX(startX),
			m_startY(startY),
			m_sizeX(sizeX),
			m_deltaX(deltaX),
			m_deltaY(deltaY),
			m_maxIterations(maxIterations)
		{
		}

	protected:
		virtual void encodeParameters() override;
		virtual void decodeParameters() override;

	private:
		friend class Tasks::MandelTask;
		uint16_t m_startRow;
		uint16_t m_endRow;
		uint16_t m_sizeX;
		double m_startX;
		double m_startY;
		double m_deltaX;
		double m_deltaY;
		uint16_t m_maxIterations;
	};
}

#endif // _MANDELMESSAGE_HPP_
