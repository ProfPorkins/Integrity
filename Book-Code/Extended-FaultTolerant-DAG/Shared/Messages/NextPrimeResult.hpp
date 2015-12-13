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
