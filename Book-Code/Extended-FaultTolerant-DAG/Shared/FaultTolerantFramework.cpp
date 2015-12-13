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

#include "FaultTolerantFramework.hpp"

#include "IRange.hpp"
#include "TaskRequestQueue.hpp"
#include "Messages/TaskRequest.hpp"
#include "Messages/TaskStatus.hpp"
#include "Messages/TerminateCommand.hpp"
#include "Threading/ThreadPool.hpp"

#include <cstdint>
#include <iostream>

// ------------------------------------------------------------------
//
// @details Prepare the class member variables and get our own TaskRequest
// message handler registered.
//
// ------------------------------------------------------------------
FaultTolerantFramework::FaultTolerantFramework() :
	m_acceptor(m_ioService, ip::tcp::endpoint(ip::tcp::v4(), 12345)),
	m_threadWork(nullptr),
	m_running(false)
{
	prepareInternalHandlers();
}

// ------------------------------------------------------------------
//
// @details This method gets the underlying Fault-Tolerant framework
// initialized and running.
//
// ------------------------------------------------------------------
bool FaultTolerantFramework::initialize()
{
	m_running = true;
	m_threadWork = std::unique_ptr<boost::asio::io_service::work>(new boost::asio::io_service::work(m_ioService));
	//
	// Create a small pool of threads from which the io_service can process requests.
	for (auto thread : IRange<uint8_t>(1, 10))
	{
		m_threadsIO.push_back(std::unique_ptr<std::thread>(new std::thread(
			[this]()
			{
				m_ioService.run();
			})));
	}

	//
	// Initialize the task request queue with the the server object
	TaskRequestQueue::instance()->initialize(&m_ioService, &m_servers);

	//
	// Go into our loop waiting for compute servers to connect
	handleNewConnection();

	return true;
}

// ------------------------------------------------------------------
//
// @details Instruct the framework to be gracefully shut down.
//
// ------------------------------------------------------------------
void FaultTolerantFramework::terminate()
{
	m_running = false;
	TaskRequestQueue::instance()->terminate();

	//
	// Send shutdown messages to all the connected servers
	std::atomic<std::size_t> sendRemaining;
	sendRemaining = m_servers.getServers().size();

	for (auto server : m_servers.getServers())
	{
		auto terminate = std::make_shared<Messages::TerminateCommand>();
		Messages::send(terminate, server.second.socket, m_ioService,
			[&sendRemaining](bool)
			{
				sendRemaining--;
			});
	}

	//
	// Go into a busy wait state, waiting for all of the messages to be sent.  Busy
	// wait is okay here, not trying to be performant.
	while (sendRemaining > 0)
		;

	//
	// Next, manually close all the sockets.
	for (auto server : m_servers.getServers())
	{
		server.second.socket->shutdown(boost::asio::socket_base::shutdown_both);
		server.second.socket->close();
	}

	//
	// Networking connections are all shutdown, terminate the io_service so that all
	// pending handlers are released.
	m_ioService.stop();

	//
	// Wait for the IO threads to exit.  Have to get these back by reference because unique_ptr
	// can not be copied.
	for (auto& thread : m_threadsIO)
	{
		thread->join();
	}
}

// ------------------------------------------------------------------
//
// @details Prepares the custom message handlers used by the framework
// itself.
//
// ------------------------------------------------------------------
void FaultTolerantFramework::prepareInternalHandlers()
{
	//
	// The TaskRequest handler needs access to the ServerId.
	m_messageCommand[Messages::Type::TaskRequest] =
		[this](ServerID_t serverId)
		{
			auto taskRequest = Messages::TaskRequest{};

			// Finish reading the task request and then place the
			// request on the queue so that it can be filled when a task
			// becomes available.
			Messages::read(taskRequest, m_servers.get(serverId)->socket);
			TaskRequestQueue::instance()->enqueueRequest(serverId);
		};

	//
	// The TaskStatus handler reports to the TaskRequestQueue it has received
	// a status update for the task.
	m_messageCommand[Messages::Type::TaskStatus] =
		[this](ServerID_t serverId)
		{
			auto taskStatus = Messages::TaskStatus{};

			Messages::read(taskStatus, m_servers.get(serverId)->socket);
			TaskRequestQueue::instance()->touchTask(taskStatus.getTaskId());
		};
}

// ------------------------------------------------------------------
//
// @details This method waits for an incoming server connection.  Once
// a connection is made, that server is added to the current set of
// available servers.
//
// ------------------------------------------------------------------
void FaultTolerantFramework::handleNewConnection()
{
	if (!m_running) return;

	auto socket = std::make_shared<ip::tcp::socket>(m_ioService);
	m_acceptor.async_accept(
		*socket,
		[this, socket](const boost::system::error_code& error)
		{
			if (!error)
			{
				std::cout << "Server connection established with : " << socket->remote_endpoint() << std::endl;
				//
				// Add this server to our list of currently available servers
				Server server(socket);
				m_servers.add(server);

				handleNextMessage(server.id);
			}
			else
			{
				std::cout << "error in accepting the connection" << std::endl;
			}
			//
			// We'll accept connections indefinitely, so go ahead and wait for another connection.
			handleNewConnection();
		});
}

// ------------------------------------------------------------------
//
// @details This method is used to sit on a socket and wait until a
// message type is received.  Once we know the message type, an instance
// of that type is created and the message read and processed.
//
// ------------------------------------------------------------------
void FaultTolerantFramework::handleNextMessage(ServerID_t serverId)
{
	if (!m_running)	return;

	//
	// A little bit of quick verification the server we are going to wait on
	// is even around.
	auto server = m_servers.get(serverId);
	if (!server || !server->socket->is_open()) return;

	//
	// Wait only on the message type, once we recieve that, we'll receive
	// the rest of the data for the specific message.
	server->socket->async_receive(
		boost::asio::buffer(server->messageType),
		[this, serverId, server](const boost::system::error_code& error, std::size_t bytes)
		{
			if (!error && bytes > 0)
			{
				//
				// Based upon the message type, invoke the associated handler
				auto type = static_cast<Messages::Type>(server->messageType[0]);
				if (m_messageCommand.find(type) != m_messageCommand.end())
				{
					m_messageCommand[type](serverId);
				}
				else
				{
					std::cout << "Unknown message type: " << server->messageType[0] << std::endl;
				}
				//
				// Now that we are finished processing the message, go ahead and wait for another message
				// to come through.
				handleNextMessage(serverId);
			}
			else
			{
				std::cout << "--- COMM Error ---" << std::endl;
				server->socket->shutdown(boost::asio::socket_base::shutdown_both);
				server->socket->close();
			}
		});
}
