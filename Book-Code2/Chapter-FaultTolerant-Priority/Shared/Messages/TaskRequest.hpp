#ifndef _TASKREQUEST_HPP_
#define _TASKREQUEST_HPP_

#include "MessagePBMixIn.hpp"

//
// Google Protocol Buffers cause hella warnings, ignore them
#pragma warning(push, 0)
#include "TaskRequest.pb.h"
#pragma warning(pop)

namespace Messages
{
	// -----------------------------------------------------------------
	//
	// @details This message is used to indicate an available CPU core
	// on a compute node.
	//
	// -----------------------------------------------------------------
	class TaskRequest : public MessagePBMixIn<PBMessages::TaskRequest>
	{
	public:
		TaskRequest() :
			MessagePBMixIn(Messages::Type::TaskRequest)
		{
		}
	};
}

#endif // _TASKREQUEST_HPP_
