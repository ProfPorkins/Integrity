#ifndef _MANDELFINISHEDMESSAGE_HPP_
#define _MANDELFINISHEDMESSAGE_HPP_

#include "TaskMessage.hpp"

//
// Google Protocol Buffers cause hella warnings, ignore them
#pragma warning(push, 0)
#include "MandelFinished.pb.h"
#pragma warning(pop)

namespace Tasks
{
	class MandelFinishedTask;
}

namespace Messages
{
	// -----------------------------------------------------------------
	//
	// @details This class is used to send a mandel finished task message to 
	// a compute node.
	//
	// -----------------------------------------------------------------
	class MandelFinished : public TaskMessage<PBMessages::MandelFinished>
	{
	public:
		MandelFinished() :
			TaskMessage(Messages::Type::MandelFinished)
		{
		}

		MandelFinished(uint64_t taskId) :
			TaskMessage(Messages::Type::MandelFinished, taskId)
		{
		}

	private:
		friend class Tasks::MandelFinishedTask;
	};
}

#endif // _MANDELFINISHEDMESSAGE_HPP_
