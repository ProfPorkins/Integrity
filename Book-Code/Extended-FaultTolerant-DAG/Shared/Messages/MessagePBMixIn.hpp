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

#ifndef _MESSAGEPBMIXIN_HPP_
#define _MESSAGEPBMIXIN_HPP_

#include "Message.hpp"

namespace Messages
{
	// -----------------------------------------------------------------
	//
	// @details This class provides the Protocol Buffer methods needed by
	// all derived messages.  It is written as a template class in order
	// to be agnotisic of the actual Protocol Buffer type, while allowing
	// the derived classes to pick up these methods and override them
	// from the parent Message class.
	//
	// -----------------------------------------------------------------
	template<typename T>
	class MessagePBMixIn : public Message
	{
	public:
		MessagePBMixIn(Type type) :
			Message(type)
		{
		}

	protected:
		T m_message;

		virtual uint32_t getMessageSize() override
		{
			return static_cast<uint32_t>(m_message.ByteSize());
		}

		virtual bool serializeToOstream(std::ostream* output) const override
		{
			return m_message.SerializeToOstream(output);
		}

		virtual bool parseFromIstream(std::istream* input) override
		{
			return m_message.ParseFromIstream(input);
		}
	};
}

#endif // _MESSAGEPBMIXIN_HPP_
