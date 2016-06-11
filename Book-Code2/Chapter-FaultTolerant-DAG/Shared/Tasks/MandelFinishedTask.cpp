#include "MandelFinishedTask.hpp"

#include "Shared/Messages/MandelFinished.hpp"
#include "Shared/Messages/MandelFinishedResult.hpp"

#include <iostream>
#include <memory>

namespace Tasks
{
	// -----------------------------------------------------------------
	//
	// @details Report a message to the server console reporting this
	// task has been executed.
	//
	// -----------------------------------------------------------------
	void MandelFinishedTask::execute()
	{
	}

	// -----------------------------------------------------------------
	//
	// @details Builds the message used to indicate what work is to be done
	//
	// -----------------------------------------------------------------
	std::shared_ptr<Messages::Message> MandelFinishedTask::getMessage()
	{
		return std::shared_ptr<Messages::MandelFinished>(new Messages::MandelFinished(m_id));
	}

	// ------------------------------------------------------------------
	//
	// @details Prepare the result message to send back to the client
	//
	// ------------------------------------------------------------------
	std::shared_ptr<Messages::Message> MandelFinishedTask::completeCustom(boost::asio::io_service& ioService)
	{
		//
		// Generate the message and return it to the calling code
		std::shared_ptr<Messages::MandelFinishedResult> message = std::make_shared<Messages::MandelFinishedResult>(this->getId());
		return message;
	}
}
