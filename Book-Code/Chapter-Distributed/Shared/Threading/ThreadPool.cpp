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

#include "ThreadPool.hpp"

#include "Shared/IRange.hpp"

#include <vector>

std::shared_ptr<ThreadPool> ThreadPool::m_instance = nullptr;

// ------------------------------------------------------------------
//
// @details This is the Singleton 'instance' accessor
//
// ------------------------------------------------------------------
std::shared_ptr<ThreadPool> ThreadPool::instance()
{
	if (m_instance)		return m_instance;

	//
	// We add a few extra threads to allow for some of them to be sitting around on other threads
	// to complete operations.  We could dynamically adjust this at runtime, up or down, by looking
	// at the amount of time spent waiting in the queue, but that is more than I'm willing to do
	// for this demo code :)
	unsigned int threads = std::thread::hardware_concurrency() + 4;
	m_instance = std::shared_ptr<ThreadPool>(new ThreadPool(threads));

	return m_instance;
}

// ------------------------------------------------------------------
//
// @details The constructor creates the worker threads the thread pool
// will use to process tasks.
//
// ------------------------------------------------------------------
ThreadPool::ThreadPool(uint16_t sizeInitial)
{
	for (auto thread : IRange<uint16_t>(1, sizeInitial ))
	{
		std::shared_ptr<WorkerThread> worker = std::make_shared<WorkerThread>(m_workQueue, m_eventTask);
		m_threads.insert(worker);
	}
}

// ------------------------------------------------------------------
//
// @details This places a new task on the work queue.  An event
// will be signaled to one of the waiting threads.
//
// ------------------------------------------------------------------
void ThreadPool::enqueueTask(std::shared_ptr<Tasks::Task> task)
{
	m_workQueue.enqueue(task);
	m_eventTask.notify_one();
}

// ------------------------------------------------------------------
//
// @details Shuts down all of the thread pool worker threads and removes
// the thread pool instance.
//
// ------------------------------------------------------------------
void ThreadPool::terminate()
{
	if (m_instance != nullptr)
	{
		//
		// Tell each of the workers threads in the waiting queue to terminate
		for (auto thread : m_instance->m_threads)
		{
			thread->terminate();
		}

		//
		// Signal the event so that any non-working threads will wake up and see they are
		// finished.
		m_instance->m_eventTask.notify_all();

		//
		// Wait for all the threads to complete
		for (auto thread : m_instance->m_threads)
		{
			thread->join();
		}

		//
		// Delete the thread pool instance itself
		m_instance.reset();
		m_instance = nullptr;
	}
}
