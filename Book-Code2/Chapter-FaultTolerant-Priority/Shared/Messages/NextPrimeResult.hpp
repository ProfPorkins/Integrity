#ifndef _NEXTPRIMERESULTMESSAGE_HPP_
#define _NEXTPRIMERESULTMESSAGE_HPP_

#include "ResultMessage.hpp"

//
// Google Protocol Buffers cause hella warnings, ignore them
#pragma warning(push, 0)
#include "NextPrimeResult.pb.h"
#pragma warning(pop)

namespace Messages
{
	// -----------------------------------------------------------------
	//
	// @details This is used to send the next prime computation result 
	// from the compute node back to the client.
	//
	// -----------------------------------------------------------------
	class NextPrimeResult : public ResultMessage<PBMessages::NextPrimeResult>
	{
	public:
		NextPrimeResult() :
			ResultMessage(Messages::Type::NextPrimeResult)
		{
		}

		NextPrimeResult(uint64_t taskId, uint32_t nextPrime) :
			ResultMessage(Messages::Type::NextPrimeResult, taskId)
		{
			m_message.set_nextprime(nextPrime);
		}

		uint32_t getNextPrime()		{ return m_message.nextprime(); }
	};
}

#endif // _NEXTPRIMERESULTMESSAGE_HPP_
