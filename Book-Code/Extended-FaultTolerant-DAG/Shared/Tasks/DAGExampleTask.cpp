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
