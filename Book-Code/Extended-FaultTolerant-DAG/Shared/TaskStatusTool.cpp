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

#include "TaskStatusTool.hpp"
#include "Shared/Messages/TaskStatus.hpp"

#include <iostream>

std::shared_ptr<TaskStatusTool> TaskStatusTool::m_instance = nullptr;

// -----------------------------------------------------------------
//
// @details This is the Singleton 'instance' accessor
//
// -----------------------------------------------------------------
TaskStatusTool* TaskStatusTool::instance()
{
	if (m_instance)		return m_instance.get();

	m_instance = std::shared_ptr<TaskStatusTool>(new TaskStatusTool());

	return m_instance.get();
}

// -----------------------------------------------------------------
//
// @details Records the io_service to use for communication and spins
// up a thread to perform the status reporting.
//
// -----------------------------------------------------------------
void TaskStatusTool::initialize(boost::asio::io_service* ioService, std::shared_ptr<ip::tcp::socket> socket)
{
	m_ioService = ioService;
	m_socket = socket;

	//
	// Create a thread that monitors the active tasks and sends out status messages for
	// them every so often.
	m_statusThread = std::make_shared<std::thread>(
		[this]()
		{
			auto previousTime = std::chrono::high_resolution_clock::now();
			while (!m_done)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(100));

				auto now = std::chrono::high_resolution_clock::now();
				auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(now - previousTime);
				previousTime = now;
				updateTasks(elapsedTime);
			}
		});
}

// -----------------------------------------------------------------
//
// @details Perform a graceful shutdown of the status reporting tool.
//
// -----------------------------------------------------------------
void TaskStatusTool::terminate()
{
	m_instance->m_done = true;
	m_instance->m_statusThread->join();

	m_instance.reset();
	m_instance = nullptr;
}

// -----------------------------------------------------------------
//
// @details Add a new task for status reporting.
//
// -----------------------------------------------------------------
void TaskStatusTool::addTask(uint64_t taskId)
{
	//
	// Tasks are added to a temporary container so that this method
	// can be called without blocking on the remove and update methods.
	std::lock_guard<std::mutex> lock(m_mutexAddedTasks);

	assert(m_addedTasks.find(taskId) == m_addedTasks.end());
	m_addedTasks[taskId] = { taskId, std::chrono::milliseconds(0) };
}

// -----------------------------------------------------------------
//
// @details Remove a task from status reporting.
//
// -----------------------------------------------------------------
void TaskStatusTool::removeTask(uint64_t taskId)
{
	//
	// Tasks to be removed are placed in a temporary container so that 
	// this method can be called without blocking on the add and 
	// update methods.
	std::lock_guard<std::mutex> lock(m_mutexRemovedTasks);

	m_removedTasks.push_back(taskId);
}

// -----------------------------------------------------------------
//
// @details Goes through the set of active tasks and sends out a status
// update if their status reporting time has come due.
//
// -----------------------------------------------------------------
void TaskStatusTool::updateTasks(std::chrono::milliseconds elapsedTime)
{
	std::lock_guard<std::mutex> lock(m_mutexActiveTasks);
	//
	// Add the new tasks first
	{
		std::lock_guard<std::mutex> lock(m_mutexAddedTasks);

		for (const auto& task : m_addedTasks)
		{
			m_activeTasks[task.first] = task.second;
		}
		m_addedTasks.clear();
	}
	//
	// Erase the removed tasks next
	{
		std::lock_guard<std::mutex> lock(m_mutexRemovedTasks);

		for (const auto& task : m_removedTasks)
		{
			//
			// Don't attempt to remove a task that doesn't exist.  This really
			// shouldn't happen, but does when I am manually debugging and the fault
			// tolerant detection kicks in even thoughthe server is active...I just
			// have it paused.
			if (m_activeTasks.find(task) != m_activeTasks.end())
			{
				m_activeTasks.erase(task);
			}
		}
		m_removedTasks.clear();
	}

	//
	// Finally, send updates as appropriate
	for (auto& task : m_activeTasks)
	{
		task.second.elapsedTime += elapsedTime;
		if (task.second.elapsedTime >= std::chrono::milliseconds(1000))
		{
			//
			// Generate and send a task status message for this task
			auto status = std::make_shared<Messages::TaskStatus>(task.first, PBMessages::TaskStatus_Status_Active);
			Messages::send(status, this->m_socket, *m_ioService);

			task.second.elapsedTime = std::chrono::milliseconds(0);
		}
	}
}
