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
