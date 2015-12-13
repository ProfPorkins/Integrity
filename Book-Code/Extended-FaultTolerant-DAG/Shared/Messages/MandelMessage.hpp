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

//
// Google Protocol Buffers cause hella warnings, ignore them
#pragma warning(push, 0)
#include "Mandel.pb.h"
#pragma warning(pop)

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
	class MandelMessage : public TaskMessage<PBMessages::Mandel>
	{
	public:
		MandelMessage() :
			TaskMessage(Messages::Type::MandelMessage)
		{
		}

		MandelMessage(uint64_t taskId, uint16_t startRow, uint16_t endRow, uint16_t sizeX, double startX, double startY, double deltaX, double deltaY, uint16_t maxIterations) :
			TaskMessage(Messages::Type::MandelMessage, taskId)
		{
			m_message.set_startrow(startRow);
			m_message.set_endrow(endRow);
			m_message.set_startx(startX);
			m_message.set_starty(startY);
			m_message.set_sizex(sizeX);
			m_message.set_deltax(deltaX);
			m_message.set_deltay(deltaY);
			m_message.set_maxiterations(maxIterations);
		}

	private:
		friend class Tasks::MandelTask;
	};
}

#endif // _MANDELMESSAGE_HPP_
