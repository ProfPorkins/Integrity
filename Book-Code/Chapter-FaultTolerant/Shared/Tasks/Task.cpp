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

#include "Task.hpp"

#include "Shared/Messages/TaskRequest.hpp"

#include <limits>
#include <mutex>

namespace Tasks
{
	// ------------------------------------------------------------------
	//
	// @details Standard constructor.  The only important thing that happens
	// here is that a unique id is assigned to the task.
	//
	// ------------------------------------------------------------------
	Task::Task(std::chrono::milliseconds duration, Priority priority) :
		m_duration(duration),
		m_priority(priority) 
	{
		static uint32_t currentId = 1;
		//
		// This lock ensures tasks are being assigned ids atomically
		static std::mutex myMutex;
		std::unique_lock<std::mutex> lock(myMutex);

		m_id = currentId++;

		//
		// Yes, I know this isn't a guarantee, but this is something I'm willing to live with.
		// If we go over 4 billion tasks and end up with a collision, that is when I'll
		// switch to UUIDs, until then, this is just fine.
		if (currentId == std::numeric_limits<uint32_t>::max())
		{
			currentId = 1;
		}
	}

	// ------------------------------------------------------------------
	//
	// @details Standard constructor.
	//
	// ------------------------------------------------------------------
	Task::Task(std::shared_ptr<ip::tcp::socket> socket, uint32_t id, Priority priority) :
		m_id(id),
		m_priority(priority),
		m_socket(socket),
		m_duration(std::chrono::milliseconds(0))
	{

	}

	// ------------------------------------------------------------------
	//
	// @details Sends the message over the connected socket, but placing the
	// event on the specified strand.
	//
	// ------------------------------------------------------------------
	void Task::send(std::shared_ptr<ip::tcp::socket> socket, boost::asio::strand& strand)
	{
		auto message = getMessage();
		message->send(socket, strand);
	}

	// ------------------------------------------------------------------
	//
	// @details This is a template method pattern.  The completion calls
	// into the derived class to allow it to implement its custom code
	// following the completion of the computation.  Once that is done,
	// this method sends the result and sends a new task request back to 
	// the task generation source.
	//
	// ------------------------------------------------------------------
	void Task::complete(boost::asio::io_service& ioService)
	{
		//
		// Call into the derived class and let it do whatever it wants first
		auto message = this->completeCustom(ioService);

		//
		// Post the message to the io_service to be returned back to the client.
		std::shared_ptr<ip::tcp::socket> socket = m_socket;
		ioService.post(
			[message, socket]()
		{
			message->send(socket);
		});

		//
		// We send a request for more work
		ioService.post(
			[socket]()
		{
			Messages::TaskRequest command;
			command.send(socket);
		});
	}
}
