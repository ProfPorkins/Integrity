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

#include "ComputeServer.hpp"

#include "Shared/IRange.hpp"
#include "Shared/Messages/DAGExampleMessage.hpp"
#include "Shared/Messages/MandelFinishedMessage.hpp"
#include "Shared/Messages/MandelMessage.hpp"
#include "Shared/Messages/NextPrimeMessage.hpp"
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
		Message message;
		message.read(socket);
		std::shared_ptr<Tasks::Task> task = std::make_shared<Task>(socket, message);

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
void ComputeServer::initialize(boost::asio::io_service& ioService, const std::string& ipClient, const std::string& portClient)
{
	prepareCommandMap();
	ThreadPool::instance()->initialize(&ioService);
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
	m_messageCommand[Messages::Type::MandelFinishedMessage] = [this](std::shared_ptr<ip::tcp::socket> socket) { processTask<Messages::MandelFinishedMessage, Tasks::MandelFinishedTask>(socket); };
	m_messageCommand[Messages::Type::NextPrimeMessage] = [this](std::shared_ptr<ip::tcp::socket> socket) { processTask<Messages::NextPrimeMessage, Tasks::NextPrimeTask>(socket); };
	m_messageCommand[Messages::Type::TerminateCommand] = [this](std::shared_ptr<ip::tcp::socket> socket) { processTerminateCommand(socket); };
	m_messageCommand[Messages::Type::DAGExampleMessage] = [this](std::shared_ptr<ip::tcp::socket> socket) { processTask<Messages::DAGExampleMessage, Tasks::DAGExampleTask>(socket); };
}

// -----------------------------------------------------------------
//
// @details This method make a connection to the client to let it know
// we are available as a compute server.  Once the connection is 
// established, a set of task requests are sent to indicate our
// availability to perform work.
//
// -----------------------------------------------------------------
void ComputeServer::connectToClient(boost::asio::io_service& ioService, const std::string& ipClient, const std::string& portClient)
{
	ip::tcp::resolver resolver(ioService);

	ip::tcp::resolver::query query(ipClient, portClient);
	ip::tcp::resolver::iterator iterator = resolver.resolve(query);

	bool done = false;
	while (!done)
	{
		auto socket = std::make_shared<ip::tcp::socket>(ioService);
		boost::system::error_code error;
		boost::asio::connect(*socket, iterator, error);
		if (!error)
		{
			done = true;
			std::cout << "Connection established with : " << socket->remote_endpoint() << std::endl;
			waitOnTask(socket);
			//
			// Request X tasks for each CPU core we have available, this allows there to be enough
			// tasks to keep the cores busy during transport of messages back and forth, rather than
			// serializing on message transport.
			unsigned int requests = std::max(1u, std::thread::hardware_concurrency()) * 2;
			for (auto core : IRange<unsigned int>(1, requests))
			{
				ioService.post(
					[socket]()
					{
						Messages::TaskRequest command;
						command.send(socket);
					});
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
void ComputeServer::waitOnTask(std::shared_ptr<ip::tcp::socket> socket)
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
				Messages::Type type = static_cast<Messages::Type>(m_messageType[0]);
				if (m_messageCommand.find(type) != m_messageCommand.end())
				{
					m_messageCommand[type](socket);
					std::cout << ".";
				}
				else
				{
					std::cout << "Unknown message type: " << static_cast<uint8_t>(type) << std::endl;
				}
				waitOnTask(socket);
			}
		});
}
