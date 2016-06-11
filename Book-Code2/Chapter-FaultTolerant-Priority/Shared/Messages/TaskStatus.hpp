#ifndef _TASKSTATUSMESSAGE_HPP_
#define _TASKSTATUSMESSAGE_HPP_

#include "MessagePBMixIn.hpp"

//
// Google Protocol Buffers cause hella warnings, ignore them
#pragma warning(push, 0)
#include "TaskStatus.pb.h"
#pragma warning(pop)

namespace Messages
{
	// -----------------------------------------------------------------
	//
	// @details This class is used to send the status of a task from
	// a compute server back to the client.
	//
	// -----------------------------------------------------------------
	class TaskStatus : public MessagePBMixIn<PBMessages::TaskStatus>
	{
	public:
		TaskStatus() :
			MessagePBMixIn(Messages::Type::TaskStatus)
		{
		}

		TaskStatus(uint64_t taskId, PBMessages::TaskStatus_Status status) :
			MessagePBMixIn(Messages::Type::TaskStatus)
		{
			m_message.set_taskid(taskId);
			m_message.set_status(status);
		}

		uint64_t getTaskId()						{ return m_message.taskid(); }
		PBMessages::TaskStatus_Status getStatus()	{ return m_message.status(); }
	};
}

#endif // _TASKSTATUSMESSAGE_HPP_
