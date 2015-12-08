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

#ifndef _WORKERTHREAD_HPP_
#define _WORKERTHREAD_HPP_

#include "ConcurrentPriorityQueue.hpp"
#include "Shared/Tasks/Task.hpp"

#include <atomic>
#include <condition_variable>
#include <memory>
#include <thread>

// ------------------------------------------------------------------
//
// @details This class provides the implementation for worker threads
// throughout the system.  It provides the key functionality that knows
// how to effeciently wait on a work queue, then, as tasks become
// available, it grabs the next one and works on it.
//
// ------------------------------------------------------------------
class WorkerThread
{
public:
	WorkerThread(ConcurrentPriorityQueue<std::shared_ptr<Tasks::Task>, TaskCompare>& workQueue, std::condition_variable& workQueueEvent);

	void run();
	void terminate();
	void join();

private:
	std::thread* m_thread;	// Have to manage the memory ourselves, do NOT delete when finished!
	bool m_done;

	ConcurrentPriorityQueue<std::shared_ptr<Tasks::Task>, TaskCompare>& m_workQueue;
	std::condition_variable& m_eventWorkQueue;
	static std::mutex m_mutexEventWorkQueue;	// Mutex must be shared between all threads
};

#endif // _WORKERTHREAD_HPP_
