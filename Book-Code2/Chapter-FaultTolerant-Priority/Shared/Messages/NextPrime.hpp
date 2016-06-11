#ifndef _NEXTPRIMEMESSAGE_HPP_
#define _NEXTPRIMEMESSAGE_HPP_

#include "TaskMessage.hpp"

//
// Google Protocol Buffers cause hella warnings, ignore them
#pragma warning(push, 0)
#include "NextPrime.pb.h"
#pragma warning(pop)

namespace Tasks
{
	class NextPrimeTask;
}

namespace Messages
{
	// -----------------------------------------------------------------
	//
	// @details This class is used to send a next prime task message
	// to a compute node.
	//
	// -----------------------------------------------------------------
	class NextPrime : public TaskMessage<PBMessages::NextPrime>
	{
	public:
		NextPrime() :
			TaskMessage(Messages::Type::NextPrime)
		{
		}

		NextPrime(uint64_t taskId, uint32_t lastPrime) :
			TaskMessage(Messages::Type::NextPrime, taskId)
		{
			m_message.set_lastprime(lastPrime);
		}

	private:
		friend class Tasks::NextPrimeTask;
	};
}

#endif // _NEXTPRIMEMESSAGE_HPP_
