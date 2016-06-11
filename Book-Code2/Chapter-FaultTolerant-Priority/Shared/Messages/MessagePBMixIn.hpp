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
