#ifndef _TASKMESSAGE_HPP_
#define _TASKMESSAGE_HPP_

#include "MessagePBMixIn.hpp"

namespace Messages
{
	template<typename T>
	class TaskMessage : public MessagePBMixIn<T>
	{
	public:
		TaskMessage(Type type) :
		MessagePBMixIn<T>(type)
		{
		}

		TaskMessage(Type type, uint64_t taskId) :
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

#endif // _TASKMESSAGE_HPP_
