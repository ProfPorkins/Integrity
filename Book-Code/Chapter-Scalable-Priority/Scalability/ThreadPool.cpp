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

#include "IRange.hpp"

std::shared_ptr<ThreadPool> ThreadPool::m_instance = nullptr;

// ------------------------------------------------------------------
//
// @details This is the Singleton 'instance' accessor
//
// ------------------------------------------------------------------
std::shared_ptr<ThreadPool> ThreadPool::instance()
{
	if (m_instance)		return m_instance;

	unsigned int threads = std::thread::hardware_concurrency();
	//
	// Can not use std::make_shared because of the protected constructor
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
	//
	// We add a few extra threads to allow for some of them to be sitting around on other threads
	// to complete operations.  We could dynamically adjust this at runtime, up or down, by looking
	// at the amount of time spent waiting in the queue, but that is for the 2nd edition of the book!
	for (auto thread : IRange<uint16_t>(1, sizeInitial + 4))
	{
		auto worker = std::make_shared<WorkerThread>(Task::Priority::One, m_taskQueue, m_eventPriorityOne);
		m_threads.insert(worker);
	}

	//
	// Make a Priority::Three thread
	auto worker = std::make_shared<WorkerThread>(Task::Priority::Three, m_taskQueue, m_eventPriorityThree);
	m_threads.insert(worker);
}

// ------------------------------------------------------------------
//
// @details This places a new task on the task queue.  An event
// will be signled to one of the waiting threads.
//
// ------------------------------------------------------------------
void ThreadPool::enqueueTask(std::shared_ptr<Task> task)
{
	m_taskQueue.enqueue(task);
	//
	// Notify threads that match this tasks's priority
	// NOTE: Why a switch statement and not a command pattern?  Speed, this is faster than 
	//       doing a lookup in a hash table.
	switch (task->getPriority())
	{
		case Task::Priority::Three:
			m_eventPriorityThree.notify_one();
		case Task::Priority::Two:
			m_eventPriorityTwo.notify_one();
		case Task::Priority::One:
			m_eventPriorityOne.notify_one();
	}
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
		m_instance->m_eventPriorityOne.notify_all();
		m_instance->m_eventPriorityTwo.notify_all();
		m_instance->m_eventPriorityThree.notify_all();

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
