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

#include "ConcurrentDAG.hpp"
#include "Task.hpp"

#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>

// ------------------------------------------------------------------
//
// @details This class provides the implementation for worker threads
// throughout the system.  It provides the key functionality that knows
// how to effeciently wait on a task queue, then, as work items become
// available, it grabs the next one and works on it.
//
// ------------------------------------------------------------------
class WorkerThread
{
public:
	WorkerThread(ConcurrentDAG<std::shared_ptr<Task>>& taskDAG, std::condition_variable& taskDAGEvent);

	void terminate();
	void join();

private:
	std::unique_ptr<std::thread> m_thread;
	bool m_done;

	ConcurrentDAG<std::shared_ptr<Task>>& m_taskDAG;
	std::condition_variable& m_eventTaskDAG;
	static std::mutex m_mutexEventTaskDAG;	// Mutex must be shared between all threads

	void run();
};

#endif // _WORKERTHREAD_HPP_
