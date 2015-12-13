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

#ifndef _TASKSTATUSTOOL_HPP_
#define _TASKSTATUSTOOL_HPP_

#include <atomic>
#include <chrono>
#include <memory>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <vector>

//
// Disable some compiler warnings that come from boost
#pragma warning(push)
#pragma warning(disable : 4267)
#pragma warning(disable : 4996)
#include <boost/asio.hpp>
#pragma warning(pop)

namespace ip = boost::asio::ip;

class TaskStatusTool
{
public:
	static TaskStatusTool* instance();

	void initialize(boost::asio::io_service* ioService, std::shared_ptr<ip::tcp::socket>);
	static void terminate();
	void addTask(uint64_t taskId);
	void removeTask(uint64_t taskId);

protected:
	TaskStatusTool() : m_done(false)
	{
	}

private:
	static std::shared_ptr<TaskStatusTool> m_instance;

	struct Tracking
	{
		uint64_t taskId;
		std::chrono::milliseconds elapsedTime;
	};

	boost::asio::io_service* m_ioService;
	std::shared_ptr<ip::tcp::socket> m_socket;

  std::atomic<bool> m_done;
	std::shared_ptr<std::thread> m_statusThread;

	std::unordered_map<uint64_t, Tracking> m_activeTasks;
	std::unordered_map<uint64_t, Tracking> m_addedTasks;
	std::vector<uint64_t> m_removedTasks;
	std::mutex m_mutexActiveTasks;
	std::mutex m_mutexAddedTasks;
	std::mutex m_mutexRemovedTasks;

	void updateTasks(std::chrono::milliseconds elapsedTime);
};

#endif // _TASKSTATUSTOOL_HPP_
