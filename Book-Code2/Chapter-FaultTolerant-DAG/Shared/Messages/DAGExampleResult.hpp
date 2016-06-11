#ifndef _DAGEXAMPLERESULTMESSAGE_HPP_
#define _DAGEXAMPLERESULTMESSAGE_HPP_

#include "ResultMessage.hpp"

//
// Google Protocol Buffers cause hella warnings, ignore them
#pragma warning(push, 0)
#include "DAGExampleResult.pb.h"
#pragma warning(pop)

namespace Messages
{
	// -----------------------------------------------------------------
	//
	// @details This is used to send the DAG Example result notification
	// back to the client.
	//
	// -----------------------------------------------------------------
	class DAGExampleResult : public ResultMessage<PBMessages::DAGExampleResult>
	{
	public:
		DAGExampleResult() :
			ResultMessage(Messages::Type::DAGExampleResult)
		{
		}

		DAGExampleResult(uint64_t taskId, std::string name) :
			ResultMessage(Messages::Type::DAGExampleResult, taskId)
		{
			m_message.set_name(name);
		}

		std::string getName() { return m_message.name(); }
	};
}

#endif // _DAGEXAMPLERESULTMESSAGE_HPP_
