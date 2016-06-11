#ifndef _MANDELFINISHEDRESULTMESSAGE_HPP_
#define _MANDELFINISHEDRESULTMESSAGE_HPP_

#include "ResultMessage.hpp"

//
// Google Protocol Buffers cause hella warnings, ignore them
#pragma warning(push, 0)
#include "MandelFinishedResult.pb.h"
#pragma warning(pop)

namespace Messages
{
	// -----------------------------------------------------------------
	//
	// @details This is used to send the mandle finished result notification
	// back to the client.
	//
	// -----------------------------------------------------------------
	class MandelFinishedResult : public ResultMessage<PBMessages::MandelFinishedResult>
	{
	public:
		MandelFinishedResult() :
			ResultMessage(Messages::Type::MandelFinishedResult)
		{
		}

		MandelFinishedResult(uint64_t taskId) :
			ResultMessage(Messages::Type::MandelFinishedResult, taskId)
		{
		}
	};
}

#endif // _MANDELFINISHEDRESULTMESSAGE_HPP_
