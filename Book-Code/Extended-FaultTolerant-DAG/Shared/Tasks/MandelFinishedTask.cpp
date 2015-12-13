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
