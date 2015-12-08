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

#ifndef _TASKREQUEST_HPP_
#define _TASKREQUEST_HPP_

#include "Message.hpp"

namespace Messages
{
	// ------------------------------------------------------------------
	//
	// @details This message is used to indicate an available CPU core
	// on a compute node.
	//
	// ------------------------------------------------------------------
	class TaskRequest : public Message
	{
	public:
		TaskRequest() :
			Message(Messages::Type::TaskRequest)
		{
		}

	protected:
		virtual void encodeMessage() override {}	// Nothing to encode, base class encodes the type which is all we need
		virtual void decodeMessage() override {}	// Similarly, nothing to decode, base class decodes the type
	};
}

#endif // _TASKREQUEST_HPP_
