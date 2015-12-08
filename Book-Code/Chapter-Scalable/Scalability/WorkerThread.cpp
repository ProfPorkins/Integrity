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

#include "WorkerThread.hpp"

std::mutex WorkerThread::m_mutexEventTaskQueue;	// Mutex must be shared between all threads

// ------------------------------------------------------------------
//
// @details This constructor gets the underlying thread created
// along with saving references to the task queue, task queue event,
// and the number of available threads counter.
//
// ------------------------------------------------------------------
WorkerThread::WorkerThread(ConcurrentQueue<std::shared_ptr<Task>>& taskQueue, std::condition_variable& eventTaskQueue) :
	m_taskQueue(taskQueue),
	m_eventTaskQueue(eventTaskQueue),
	m_done(false),
	m_thread(nullptr)
{
	m_thread = std::unique_ptr<std::thread>(new std::thread(&WorkerThread::run, this));
}

// ------------------------------------------------------------------
//
// @details This is the entry point method for the actual worker thread.  This
// method stays running until we are asked to voluntarily terminate.  The
// thread waits on a signal to check for something in the task queue.  If there
// is something in the queue, it goes to work.
//
// ------------------------------------------------------------------
void WorkerThread::run()
{
	while (!m_done)
	{
		std::shared_ptr<Task> task;
		if (m_taskQueue.dequeue(task))
		{
			task->execute();
			task->complete();
		}
		else
		{
			std::unique_lock<std::mutex> lock(m_mutexEventTaskQueue);
			m_eventTaskQueue.wait(lock);
		}
	}
}

// ------------------------------------------------------------------
//
// @details This is the method through which the thread is asked to voluntarily
// terminate.  Whatever task it is currently handling is first finished, then
// the thread will finally terminate.
//
// ------------------------------------------------------------------
void WorkerThread::terminate()
{
	m_done = true;
}

// ------------------------------------------------------------------
//
// @details Pass through to allow the thread to be joined
//
// ------------------------------------------------------------------
void WorkerThread::join()
{
	m_thread->join();
}
