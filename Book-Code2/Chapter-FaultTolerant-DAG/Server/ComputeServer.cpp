#include "ComputeServer.hpp"

#include "Shared/IRange.hpp"
#include "Shared/TaskStatusTool.hpp"
#include "Shared/Messages/DAGExample.hpp"
#include "Shared/Messages/MandelFinished.hpp"
#include "Shared/Messages/MandelMessage.hpp"
#include "Shared/Messages/NextPrime.hpp"
#include "Shared/Messages/TaskRequest.hpp"
#include "Shared/Tasks/DAGExampleTask.hpp"
#include "Shared/Tasks/MandelFinishedTask.hpp"
#include "Shared/Tasks/MandelTask.hpp"
#include "Shared/Tasks/NextPrimeTask.hpp"
#include "Shared/Threading/ThreadPool.hpp"

#include <iostream>
#include <thread>

namespace
{
	// ------------------------------------------------------------------
	//
	// @details Finishes reading a task message, then places the task onto 
	// the thread pool for execution.
	//
	// ------------------------------------------------------------------
	template <typename Message, typename Task>
	void processTask(std::shared_ptr<ip::tcp::socket> socket)
	{
		auto message = Message{};
		Messages::read(message, socket);
		auto task = std::make_shared<Task>(socket, message);

		//
		// Add this to the status reporting tool so the client is able
		// to track the status of the task.
		TaskStatusTool::instance()->addTask(task->getId());

		ThreadPool::instance()->enqueueTask(task);
	}

	// ------------------------------------------------------------------
	//
	// @details When the terminate command is received, do a somewhat
	// awkward, but not completely terrible, application shutdown.
	//
	// ------------------------------------------------------------------
	void processTerminateCommand(std::shared_ptr<ip::tcp::socket> socket)
	{
		//
		// Gracefully shutdown the Task status reporting tool and the thread pool
		TaskStatusTool::terminate();
		//
		// Need to gracefully shut down the thread pool.
		ThreadPool::terminate();

		//
		// Finally, shutdown the io_service
		socket->get_io_service().stop();
	}
}

// -----------------------------------------------------------------
//
// @details Prepares the command map used by the message handler.
// Initializes the thread pool with the io_service.
// Makes the connection to the client.
//
// -----------------------------------------------------------------
void ComputeServer::initialize(boost::asio::io_service* ioService, const std::string& ipClient, const std::string& portClient)
{
	prepareCommandMap();
	ThreadPool::instance()->initialize(ioService);
	connectToClient(ioService, ipClient, portClient);
}

// -----------------------------------------------------------------
//
// @details We are using a Command Pattern to handle the different
// types of messages the client can receive.  This method is where
// the command map is prepared.
//
// -----------------------------------------------------------------
void ComputeServer::prepareCommandMap()
{
	m_messageCommand[Messages::Type::MandelMessage] = [this](std::shared_ptr<ip::tcp::socket> socket) { processTask<Messages::MandelMessage, Tasks::MandelTask>(socket); };
	m_messageCommand[Messages::Type::MandelFinished] = [this](std::shared_ptr<ip::tcp::socket> socket) { processTask<Messages::MandelFinished, Tasks::MandelFinishedTask>(socket); };
	m_messageCommand[Messages::Type::NextPrime] = [this](std::shared_ptr<ip::tcp::socket> socket) { processTask<Messages::NextPrime, Tasks::NextPrimeTask>(socket); };
	m_messageCommand[Messages::Type::TerminateCommand] = [this](std::shared_ptr<ip::tcp::socket> socket) { processTerminateCommand(socket); };
	m_messageCommand[Messages::Type::DAGExample] = [this](std::shared_ptr<ip::tcp::socket> socket) { processTask<Messages::DAGExample, Tasks::DAGExampleTask>(socket); };
}

// -----------------------------------------------------------------
//
// @details This method make a connection to the client to let it know
// we are available as a compute server.  Once the connection is 
// established, a set of task requests are sent to indicate our
// availability to perform work.
//
// -----------------------------------------------------------------
void ComputeServer::connectToClient(boost::asio::io_service* ioService, const std::string& ipClient, const std::string& portClient)
{
	ip::tcp::resolver resolver(*ioService);

	ip::tcp::resolver::query query(ipClient, portClient);
	ip::tcp::resolver::iterator iterator = resolver.resolve(query);

	auto done = bool{ false };
	while (!done)
	{
		auto socket = std::make_shared<ip::tcp::socket>(*ioService);
		auto error = boost::system::error_code{};
		boost::asio::connect(*socket, iterator, error);
		if (!error)
		{
			done = true;
			std::cout << "Connection established with : " << socket->remote_endpoint() << std::endl;

			//
			// Initialize the task status reporting tool
			TaskStatusTool::instance()->initialize(ioService, socket);
			//
			// Begin waiting for incoming tasks...do this before sending any task requests to prevent any possible
			// race conditions.
			handleNextTask(socket);
			//
			// Request X tasks for each CPU core we have available, this allows there to be enough
			// tasks to keep the cores busy during transport of messages back and forth, rather than
			// serializing on message transport.
			auto requests = unsigned int{ std::max(1u, std::thread::hardware_concurrency()) * 2 };
			auto command = std::make_shared<Messages::TaskRequest>();
			for (auto core : IRange<unsigned int>(1, requests))
			{
				Messages::send(command, socket, *ioService);
			}
		}
	}
}

// -----------------------------------------------------------------
//
// @details This method is used to wait for a task message to come
// down from an already connected client, via the specified socket.
// Once a task message comes, it is processed by the associated task
// handler.
//
// -----------------------------------------------------------------
void ComputeServer::handleNextTask(std::shared_ptr<ip::tcp::socket> socket)
{
	if (!socket->is_open()) return;

	//
	// Wait only on the message type, once we recieve that, we'll receive
	// the rest of the data for the specific message.
	socket->async_receive(
		boost::asio::buffer(m_messageType),
		[this, socket](const boost::system::error_code& error, std::size_t bytes)
		{
			if (!error)
			{
				//
				// Based upon the message type, invoke the associated handler
				auto type = static_cast<Messages::Type>(m_messageType[0]);
				if (m_messageCommand.find(type) != m_messageCommand.end())
				{
					m_messageCommand[type](socket);
					std::cout << ".";
				}
				else
				{
					std::cout << "Unknown message type: " << static_cast<uint8_t>(type) << std::endl;
				}
				handleNextTask(socket);
			}
		});
}
