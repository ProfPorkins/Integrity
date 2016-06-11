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
