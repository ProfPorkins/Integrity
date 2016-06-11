#ifndef _DAGEXAMPLEMESSAGE_HPP_
#define _DAGEXAMPLEMESSAGE_HPP_

#include "TaskMessage.hpp"

//
// Google Protocol Buffers cause hella warnings, ignore them
#pragma warning(push, 0)
#include "DAGExample.pb.h"
#pragma warning(pop)

namespace Tasks
{
	class DAGExampleTask;
}

namespace Messages
{
	// -----------------------------------------------------------------
	//
	// @details This class is used to send a DAG Example task message
	// to a compute node.
	//
	// -----------------------------------------------------------------
	class DAGExample : public TaskMessage<PBMessages::DAGExample>
	{
	public:
		DAGExample() :
			TaskMessage(Messages::Type::DAGExample)
		{
		}

		DAGExample(uint64_t taskId, std::string name) :
			TaskMessage(Messages::Type::DAGExample, taskId)
		{
			m_message.set_name(name);
		}

	private:
		friend class Tasks::DAGExampleTask;
	};
}

#endif // _DAGEXAMPLEMESSAGE_HPP_
