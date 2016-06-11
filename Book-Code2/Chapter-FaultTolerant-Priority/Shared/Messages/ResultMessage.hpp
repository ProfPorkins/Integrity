#ifndef _RESULTMESSAGE_HPP_
#define _RESULTMESSAGE_HPP_

#include "MessagePBMixIn.hpp"

namespace Messages
{
	template<typename T>
	class ResultMessage : public MessagePBMixIn<T>
	{
	public:
		ResultMessage(Type type) :
		MessagePBMixIn<T>(type)
		{
		}

		ResultMessage(Type type, uint64_t taskId) :
		MessagePBMixIn<T>(type)
		{
		this->m_message.set_taskid(taskId);
		}

		uint64_t getTaskId()
		{
		return this->m_message.taskid();
		}
	};
}

#endif // _RESULTMESSAGE_HPP_
