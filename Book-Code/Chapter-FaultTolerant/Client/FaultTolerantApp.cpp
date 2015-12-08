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

#include "FaultTolerantApp.hpp"

#include "Shared/IRange.hpp"
#include "Shared/Messages/MandelResultMessage.hpp"
#include "Shared/Messages/NextPrimeMessage.hpp"
#include "Shared/Messages/NextPrimeResultMessage.hpp"
#include "Shared/Messages/TaskRequest.hpp"
#include "Shared/Messages/TerminateCommand.hpp"
#include "Shared/Tasks/NextPrimeTask.hpp"
#include "Shared/Threading/ThreadPool.hpp"
#include "TaskRequestQueue.hpp"

#include <iostream>

// ------------------------------------------------------------------
//
// @details Initialize the Mandelbrot region, the mandelbrot rendering
// bitmap, and the message response command map.
//
// ------------------------------------------------------------------
FaultTolerantApp::FaultTolerantApp(HWND hwnd, uint16_t sizeX, uint16_t sizeY) :
	m_acceptor(m_ioService, ip::tcp::endpoint(ip::tcp::v4(), 12345)),
	m_threadWork(nullptr),
	m_running(true),
	m_reportPrime(false)
{
	prepareCommandMap();

	//
	// Get the mandelbrot code created and initialized
	m_mandelbrot = std::make_shared<Mandelbrot>(hwnd, sizeX, sizeY);
}

// ------------------------------------------------------------------
//
// @details Get the io_service thread pool created, initialize
// the TaskRequestQueue, begin waiting for connections from the 
// compute servers, finally, generate the initial prime number request.
//
// ------------------------------------------------------------------
bool FaultTolerantApp::initialize()
{
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

	//
	// Generate the initial next prime request, we are running them
	// at Priority::Three to demonstrate we can distribute high-priority
	// tasks ahead of low-priority ones.
	std::shared_ptr<Tasks::NextPrimeTask> task = std::make_shared<Tasks::NextPrimeTask>(1, Tasks::Task::Priority::Three);
	TaskRequestQueue::instance()->enqueueTask(task);

	return true;
}

// ------------------------------------------------------------------
//
// @details Allows the application to be gracefully shut down.
//
// ------------------------------------------------------------------
void FaultTolerantApp::terminate()
{
	m_running = false;
	TaskRequestQueue::instance()->terminate();

	//
	// Send shutdown messages to all the connected servers
	std::atomic<std::size_t> sendRemaining = m_servers.getServers().size();
	for (auto server : m_servers.getServers())
	{
		Messages::TerminateCommand terminate;
		terminate.send(server.second.socket,
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
// @details This is called as often as possible to render the Mandelbrot
// set.  If any change in the view has been requested, a new task is
// started that will cause the new view to be rendered.
//
// ------------------------------------------------------------------
void FaultTolerantApp::pulse()
{
	m_mandelbrot->update();

	//
	// Render the most recently computed prime number
	if (m_reportPrime)
	{
		std::cout << "Next Prime: " << m_lastPrime << std::endl;
		m_reportPrime = false;
	}
}

// ------------------------------------------------------------------
//
// @details We are using a Command Pattern to handle the different
// types of messages the client can receive.  This method is where
// the command map is prepared.
//
// ------------------------------------------------------------------
void FaultTolerantApp::prepareCommandMap()
{
	m_messageCommand[Messages::Type::TaskRequest] = [this](ServerID_t serverId) { processTaskRequest(serverId); };
	m_messageCommand[Messages::Type::MandelResult] = [this](ServerID_t serverId) { processMandelResult(serverId); };
	m_messageCommand[Messages::Type::NextPrimeResult] = [this](ServerID_t serverId) { processNextPrimeResult(serverId); };
}

// ------------------------------------------------------------------
//
// @details This method waits for an incoming server connection.  Once
// a connection is made, that server is added to the current set of
// available servers.
//
// ------------------------------------------------------------------
void FaultTolerantApp::handleNewConnection()
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
void FaultTolerantApp::handleNextMessage(ServerID_t serverId)
{
	if (!m_running)	return;

	//
	// A little bit of quick verification the server we are going to wait on
	// is even around.
	boost::optional<Server&> server = m_servers.get(serverId);
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
				Messages::Type type = static_cast<Messages::Type>(server->messageType[0]);
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
			}
		});
}

// ------------------------------------------------------------------
//
// @details Finishes reading the task request and then places the
// request on the queue so that it can be filled when a task
// becomes available.
//
// ------------------------------------------------------------------
void FaultTolerantApp::processTaskRequest(ServerID_t serverId)
{
	Messages::TaskRequest taskRequest;
	taskRequest.read(m_servers.get(serverId)->socket);
	TaskRequestQueue::instance()->enqueueRequest(serverId);
}

// ------------------------------------------------------------------
//
// @details Finishes reading the mandel result and then gets those
// data copied into the displayable image.
//
// ------------------------------------------------------------------
void FaultTolerantApp::processMandelResult(ServerID_t serverId)
{
	Messages::MandelResultMessage taskResult;
	taskResult.read(m_servers.get(serverId)->socket);

	//
	// Let the task queue know this task result has been recieved
	if (TaskRequestQueue::instance()->finalizeTask(taskResult.getTaskId()))
	{
		m_mandelbrot->processMandelResult(taskResult);
	}
}

// ------------------------------------------------------------------
//
// @details Finishes reading the next prime result, displays it, and
// then generates a request to compute the next prime number.
//
// ------------------------------------------------------------------
void FaultTolerantApp::processNextPrimeResult(ServerID_t serverId)
{
	Messages::NextPrimeResultMessage taskResult;
	taskResult.read(m_servers.get(serverId)->socket);

	//
	// Let the work queue know this task result has been recieved
	if (TaskRequestQueue::instance()->finalizeTask(taskResult.getTaskId()))
	{
		m_lastPrime = taskResult.getNextPrime();
		m_reportPrime = true;
		//
		// Generate a task request for the next prime, we are running them
		// at Priority::Three to demonstrate we can distribute high-priority
		// tasks ahead of low-priority ones.
		std::shared_ptr<Tasks::NextPrimeTask> task = std::make_shared<Tasks::NextPrimeTask>(taskResult.getNextPrime(), Tasks::Task::Priority::Three);
		TaskRequestQueue::instance()->enqueueTask(task);
	}
}
