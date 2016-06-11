#include "Task.hpp"

#include "Shared/Messages/TaskRequest.hpp"

#include <limits>
#include <mutex>

namespace Tasks
{
	// -----------------------------------------------------------------
	//
	// @details Standard constructor.  The only important thing that happens
	// here is that a unique id is assigned to the task.
	//
	// -----------------------------------------------------------------
	Task::Task(Priority priority) :
		m_priority(priority)
	{
		static uint64_t currentId = 1;
		//
		// This lock ensures tasks are being assigned ids atomically
		static std::mutex myMutex;
		std::lock_guard<std::mutex> lock(myMutex);

		m_id = currentId++;

		//
		// Yes, I know this isn't a guarantee, but this is something I'm willing to live with.
		// If we go over 64 bits worth of integers and end up with a collision, that is when I'll
		// switch to UUIDs, until then, this is just fine.
		if (currentId == std::numeric_limits<uint32_t>::max())
		{
			currentId = 1;
		}
	}

	// -----------------------------------------------------------------
	//
	// @details Standard constructor.  This constructor has no need to understand
	// priority because it is only used at the compute servers, not in the client.
	// By the time the task makes it to the compute server, priority has
	// already been used.
	//
	// -----------------------------------------------------------------
	Task::Task(std::shared_ptr<ip::tcp::socket> socket, uint64_t id) :
		m_id(id),
		m_socket(socket)
	{
	}

	// -----------------------------------------------------------------
	//
	// @details Sends the message over the connected socket, but placing the
	// event on the specified strand.
	//
	// -----------------------------------------------------------------
	void Task::send(std::shared_ptr<ip::tcp::socket> socket, boost::asio::strand& strand)
	{
		auto message = getMessage();
		Messages::send(message, socket, strand);
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
		Messages::send(message, m_socket, ioService);

		//
		// We send a request for more work
		auto request = std::make_shared<Messages::TaskRequest>();
		Messages::send(request, m_socket, ioService);
	}
}
