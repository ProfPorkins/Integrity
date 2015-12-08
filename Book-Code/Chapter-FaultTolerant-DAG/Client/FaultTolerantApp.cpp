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
#include "Shared/Messages/DAGExampleMessage.hpp"
#include "Shared/Messages/DAGExampleResultMessage.hpp"
#include "Shared/Messages/MandelFinishedResultMessage.hpp"
#include "Shared/Messages/MandelResultMessage.hpp"
#include "Shared/Messages/NextPrimeMessage.hpp"
#include "Shared/Messages/NextPrimeResultMessage.hpp"
#include "Shared/Messages/TaskRequest.hpp"
#include "Shared/Messages/TerminateCommand.hpp"
#include "Shared/Tasks/DAGExampleTask.hpp"
#include "Shared/Tasks/NextPrimeTask.hpp"
#include "Shared/Threading/ThreadPool.hpp"
#include "TaskRequestQueue.hpp"

#include <iostream>

namespace 
{
	// ------------------------------------------------------------------
	//
	// @details This method prepares tasks based upon the DAG (Figure 6.1) 
	// from the book.
	//
	// ------------------------------------------------------------------
	void startChapterDemo()
	{
		TaskRequestQueue::instance()->beginGroup();

		auto task1 = std::make_shared<Tasks::DAGExampleTask>("1");
		auto task2 = std::make_shared<Tasks::DAGExampleTask>("2");
		auto task3 = std::make_shared<Tasks::DAGExampleTask>("3");
		auto task4 = std::make_shared<Tasks::DAGExampleTask>("4");
		auto task5 = std::make_shared<Tasks::DAGExampleTask>("5");
		auto task6 = std::make_shared<Tasks::DAGExampleTask>("6");
		auto task7 = std::make_shared<Tasks::DAGExampleTask>("7");
		auto task8 = std::make_shared<Tasks::DAGExampleTask>("8");
		auto task9 = std::make_shared<Tasks::DAGExampleTask>("9");
		auto task10 = std::make_shared<Tasks::DAGExampleTask>("10");
		auto task11 = std::make_shared<Tasks::DAGExampleTask>("11");
		auto task12 = std::make_shared<Tasks::DAGExampleTask>("12");
		auto task13 = std::make_shared<Tasks::DAGExampleTask>("13");
		auto task14 = std::make_shared<Tasks::DAGExampleTask>("14");
		auto task15 = std::make_shared<Tasks::DAGExampleTask>("15");

		auto task16 = std::make_shared<Tasks::DAGExampleTask>("16");
		auto task17 = std::make_shared<Tasks::DAGExampleTask>("17");
		auto task18 = std::make_shared<Tasks::DAGExampleTask>("18");
		auto task19 = std::make_shared<Tasks::DAGExampleTask>("19");
		auto task20 = std::make_shared<Tasks::DAGExampleTask>("20");
		auto task21 = std::make_shared<Tasks::DAGExampleTask>("21");
		auto task22 = std::make_shared<Tasks::DAGExampleTask>("22");
		auto task23 = std::make_shared<Tasks::DAGExampleTask>("23");
		auto task24 = std::make_shared<Tasks::DAGExampleTask>("24");
		auto task25 = std::make_shared<Tasks::DAGExampleTask>("25");
		auto task26 = std::make_shared<Tasks::DAGExampleTask>("26");
		auto task27 = std::make_shared<Tasks::DAGExampleTask>("27");
		auto task28 = std::make_shared<Tasks::DAGExampleTask>("28");
		auto task29 = std::make_shared<Tasks::DAGExampleTask>("29");

		TaskRequestQueue::instance()->enqueueTask(task2, task1);
		TaskRequestQueue::instance()->enqueueTask(task3, task1);
		TaskRequestQueue::instance()->enqueueTask(task4, task2);
		TaskRequestQueue::instance()->enqueueTask(task5, task2);
		TaskRequestQueue::instance()->enqueueTask(task6, task3);
		TaskRequestQueue::instance()->enqueueTask(task7, task3);
		TaskRequestQueue::instance()->enqueueTask(task8, task4);
		TaskRequestQueue::instance()->enqueueTask(task9, task4);
		TaskRequestQueue::instance()->enqueueTask(task10, task5);
		TaskRequestQueue::instance()->enqueueTask(task11, task5);
		TaskRequestQueue::instance()->enqueueTask(task12, task6);
		TaskRequestQueue::instance()->enqueueTask(task13, task6);
		TaskRequestQueue::instance()->enqueueTask(task14, task7);
		TaskRequestQueue::instance()->enqueueTask(task15, task7);

		TaskRequestQueue::instance()->enqueueTask(task1, task16);
		TaskRequestQueue::instance()->enqueueTask(task1, task17);
		TaskRequestQueue::instance()->enqueueTask(task16, task18);
		TaskRequestQueue::instance()->enqueueTask(task16, task19);
		TaskRequestQueue::instance()->enqueueTask(task17, task20);
		TaskRequestQueue::instance()->enqueueTask(task17, task21);
		TaskRequestQueue::instance()->enqueueTask(task18, task22);
		TaskRequestQueue::instance()->enqueueTask(task18, task23);
		TaskRequestQueue::instance()->enqueueTask(task19, task24);
		TaskRequestQueue::instance()->enqueueTask(task19, task25);
		TaskRequestQueue::instance()->enqueueTask(task20, task26);
		TaskRequestQueue::instance()->enqueueTask(task20, task27);
		TaskRequestQueue::instance()->enqueueTask(task21, task28);
		TaskRequestQueue::instance()->enqueueTask(task21, task29);

		TaskRequestQueue::instance()->endGroup();
	}
}

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
// @details Create connections to each of the compute servers we'll be 
// using and begin waiting on a message from them.
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
	// Generate the initial next prime request
	std::shared_ptr<Tasks::NextPrimeTask> task = std::make_shared<Tasks::NextPrimeTask>(1);
	TaskRequestQueue::instance()->enqueueTask(task);

	startChapterDemo();

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
	m_messageCommand[Messages::Type::MandelFinishedResult] = [this](ServerID_t serverId) { processMandelFinishedResult(serverId); };
	m_messageCommand[Messages::Type::NextPrimeResult] = [this](ServerID_t serverId) { processNextPrimeResult(serverId); };
	m_messageCommand[Messages::Type::DAGExampleResult] = [this](ServerID_t serverId) { processDAGExampleResult(serverId); };
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
	if (TaskRequestQueue::instance()->finalizeTask(taskResult.getTaskId(), true, true))
	{
		m_mandelbrot->processMandelResult(taskResult);
	}
	else
	{
		std::cout << "Not finalized" << std::endl;
	}
}

// ------------------------------------------------------------------
//
// @details Finishes reading the mandel finished result and invokes
// the appropriate handler in the Mandelbrot class to signal it is
// now complete.
//
// ------------------------------------------------------------------
void FaultTolerantApp::processMandelFinishedResult(ServerID_t serverId)
{
	Messages::MandelFinishedResultMessage taskResult;
	taskResult.read(m_servers.get(serverId)->socket);

	//
	// Let the task queue know this task result has been recieved
	if (TaskRequestQueue::instance()->finalizeTask(taskResult.getTaskId(), true, true))
	{
		m_mandelbrot->processMandelFinishedResult(taskResult);
	}
	else
	{
		std::cout << "Not finalized" << std::endl;
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
	if (TaskRequestQueue::instance()->finalizeTask(taskResult.getTaskId(), true, true))
	{
		m_lastPrime = taskResult.getNextPrime();
		m_reportPrime = true;
		//
		// Generate a task request for the next prime
		std::shared_ptr<Tasks::NextPrimeTask> task = std::make_shared<Tasks::NextPrimeTask>(taskResult.getNextPrime());
		TaskRequestQueue::instance()->enqueueTask(task);
	}
}

// ------------------------------------------------------------------
//
// @details Finishes reading the DAG Example task and reports its
// name to the console.
//
// ------------------------------------------------------------------
void FaultTolerantApp::processDAGExampleResult(ServerID_t serverId)
{
	Messages::DAGExampleResultMessage taskResult;
	taskResult.read(m_servers.get(serverId)->socket);

	//
	// Let the work queue know this task result has been recieved
	if (TaskRequestQueue::instance()->finalizeTask(taskResult.getTaskId(), true, true))
	{
		std::cout << "DAG Example Task: " << taskResult.getName() << std::endl;
	}
}
