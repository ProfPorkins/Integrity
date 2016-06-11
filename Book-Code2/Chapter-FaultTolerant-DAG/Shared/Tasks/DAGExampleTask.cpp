#include "DAGExampleTask.hpp"

#include "Shared/Messages/DAGExample.hpp"
#include "Shared/Messages/DAGExampleResult.hpp"

#include <chrono>
#include <memory>
#include <thread>

namespace Tasks
{
	// -----------------------------------------------------------------
	//
	// @details Sleep for a bit.
	//
	// -----------------------------------------------------------------
	void DAGExampleTask::execute()
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(4000));
	}

	// -----------------------------------------------------------------
	//
	// @details Builds the message used to indicate what work is to be done
	//
	// -----------------------------------------------------------------
	std::shared_ptr<Messages::Message> DAGExampleTask::getMessage()
	{
		return std::make_shared<Messages::DAGExample>(this->getId(), m_name);
	}

	// ------------------------------------------------------------------
	//
	// @details Prepare the result message to send back to the client
	//
	// ------------------------------------------------------------------
	std::shared_ptr<Messages::Message> DAGExampleTask::completeCustom(boost::asio::io_service& ioService)
	{
		//
		// Generate the message and return it to the calling code
		std::shared_ptr<Messages::DAGExampleResult> message = std::make_shared<Messages::DAGExampleResult>(this->getId(), m_name);
		return message;
	}
}
