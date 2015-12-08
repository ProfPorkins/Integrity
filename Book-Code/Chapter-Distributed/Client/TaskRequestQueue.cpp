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

#include "TaskRequestQueue.hpp"

#include "Shared/Messages/MandelMessage.hpp"

#include <chrono>
#include <iostream>

#include <boost/optional.hpp>

std::shared_ptr<TaskRequestQueue> TaskRequestQueue::m_instance = nullptr;

// ------------------------------------------------------------------
//
// @details Protected constructor used to initialize the single
// instance of the queue/distributer.
//
// ------------------------------------------------------------------
TaskRequestQueue::TaskRequestQueue() :
	m_distributerDone(false)
{
}

// ------------------------------------------------------------------
//
// @details This is the Singleton ::instance accessor method.  The first
// time this is called, a new thread is created that is used to distribute
// any tasks out to the waiting compute servers.
//
// ------------------------------------------------------------------
std::shared_ptr<TaskRequestQueue> TaskRequestQueue::instance()
{
	if (m_instance)		return m_instance;

	//
	// Can't use std::make_shared because of the protected constructor
	m_instance = std::shared_ptr<TaskRequestQueue>(new TaskRequestQueue());

	//
	// Create a thread that listens to the work queue and distributes those tasks to pending task requests
	m_instance->m_distributer = std::make_shared<std::thread>(&TaskRequestQueue::distribute, m_instance);
	return m_instance;
}

// ------------------------------------------------------------------
//
// @details This allows the singleton to be initialized with the set
// of compute servers.
//
// ------------------------------------------------------------------
void TaskRequestQueue::initialize(boost::asio::io_service* ioService, ServerSet* servers)
{
	m_ioService = ioService;
	m_servers = servers;
}

// ------------------------------------------------------------------
//
// @details Tells the work distributer to stop and blocks until that
// thread completes.
//
// ------------------------------------------------------------------
void TaskRequestQueue::terminate()
{
	m_distributerDone = true;
	m_eventTask.notify_all();
	m_eventRequest.notify_all();

	m_distributer->join();
}

// ------------------------------------------------------------------
//
// @details This places a new task request on the request queue.  An event
// will be signaled to one of the waiting threads.
//
// ------------------------------------------------------------------
void TaskRequestQueue::enqueueRequest(ServerID_t request)
{
	std::lock_guard<std::mutex> lock(m_mutexRequest);

	m_queueRequest.push(request);
	m_eventRequest.notify_all();
}

// ------------------------------------------------------------------
//
// @details This places a new task on the working queue.  An event
// will be signled to one of the waiting threads.
//
// ------------------------------------------------------------------
void TaskRequestQueue::enqueueTask(std::shared_ptr<Tasks::Task> task)
{
	m_queueTasks.enqueue(task);
	m_eventTask.notify_all();
}

// ------------------------------------------------------------------
//
// @details This is the entry point method for the actual work distribution
// thread.  This method stays running until we are asked to voluntarily terminate.  
// The thread waits on a signal to check for something in the work queue.  If there
// is something in the work queue and an available task request, work is sent to
// the server with an open request.
//
// ------------------------------------------------------------------
void TaskRequestQueue::distribute()
{
	while (!m_distributerDone)
	{
		//
		// Look at the new work queue and pull something from there if possible
		auto task = m_queueTasks.dequeue();
		if (task)
		{
			fillRequest(task.get());
		}
		else
		{
			std::unique_lock<std::mutex> lock(m_mutexEventTask);
			m_eventTask.wait(lock, [&] { return m_queueTasks.size() == 0; });
		}
	}
}

// ------------------------------------------------------------------
//
// @details This method is used to send a task to a compute server
// that has indicated (via a task request) that it is available to
// do some work.
//
// ------------------------------------------------------------------
void TaskRequestQueue::fillRequest(std::shared_ptr<Tasks::Task> task)
{
	ServerID_t serverId = 0;
	bool done = false;
	while (!done)
	{
		//
		// Try to get a matching request
		{
			std::lock_guard<std::mutex> lockRequest(m_mutexRequest);
			if (!m_queueRequest.empty())
			{
				serverId = m_queueRequest.front();
				m_queueRequest.pop();
				done = true;
			}
		}
		if (!done)
		{
			std::unique_lock<std::mutex> lock(m_mutexEventRequest);
			m_eventRequest.wait(lock);
		}
	}

	m_ioService->post(
		[this, serverId, task]()
		{
			task->send(m_servers->get(serverId)->socket, *m_servers->get(serverId)->strand);
		});
}
