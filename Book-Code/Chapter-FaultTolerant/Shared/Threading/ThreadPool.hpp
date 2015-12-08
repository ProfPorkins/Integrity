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

#ifndef _THREADPOOL_HPP_
#define _THREADPOOL_HPP_

#include "ConcurrentPriorityQueue.hpp"
#include "Shared/Tasks/Task.hpp"
#include "WorkerThread.hpp"

#include <condition_variable>
#include <memory>
#include <set>
#include <vector>

//
// Disable some compiler warnings that come from boost
#pragma warning(push)
#pragma warning(disable : 4267)
#pragma warning(disable : 4996)
#include <boost/asio.hpp>
#pragma warning(pop)

namespace ip = boost::asio::ip;

// ------------------------------------------------------------------
//
// @details Provides an implementation of a Thread Pool.  It is intended to
// be used to create & manage worker threads that handle all tasks throughout
// the system.
//
// ------------------------------------------------------------------
class ThreadPool
{
public:
	static std::shared_ptr<ThreadPool> instance();

	void initialize(boost::asio::io_service* ioService) { m_ioService = ioService; }
	void enqueueTask(std::shared_ptr<Tasks::Task> task);
	static void terminate();

	boost::asio::io_service* getIOService() { return m_ioService; }

protected:
	ThreadPool(uint16_t sizeInitial);

private:
	static std::shared_ptr<ThreadPool> m_instance;
	boost::asio::io_service* m_ioService;

	std::set<std::shared_ptr<WorkerThread>> m_threads;

	ConcurrentPriorityQueue<std::shared_ptr<Tasks::Task>, TaskCompare> m_workQueue;
	std::condition_variable m_eventTask;
};

#endif // _THREADPOOL_HPP_
