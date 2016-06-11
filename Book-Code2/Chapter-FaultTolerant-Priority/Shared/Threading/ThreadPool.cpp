#include "ThreadPool.hpp"

#include "Shared/IRange.hpp"

std::shared_ptr<ThreadPool> ThreadPool::m_instance = nullptr;

// -----------------------------------------------------------------
//
// @details This is the Singleton 'instance' accessor
//
// -----------------------------------------------------------------
std::shared_ptr<ThreadPool> ThreadPool::instance()
{
	if (m_instance)		return m_instance;

	//
	// We add a few extra threads to allow for some of them to be sitting around on other threads
	// to complete operations.  We could dynamically adjust this at runtime, up or down, by looking
	// at the amount of time spent waiting in the queue, but that is more than I'm willing to do
	// for this demo code :)
	auto threads = uint16_t{ static_cast<uint16_t>(std::thread::hardware_concurrency() + 4) };
	m_instance = std::shared_ptr<ThreadPool>(new ThreadPool(threads));

	return m_instance;
}

// -----------------------------------------------------------------
//
// @details The constructor creates the worker threads the thread pool
// will use to process tasks.
//
// -----------------------------------------------------------------
ThreadPool::ThreadPool(uint16_t sizeInitial)
{
	for (auto thread : IRange<uint16_t>(1, sizeInitial))
	{
		auto worker = std::make_shared<WorkerThread>(m_workQueue, m_eventWorkQueue, m_mutexWorkQueue);
		m_threads.insert(worker);
	}
}

// -----------------------------------------------------------------
//
// @details This places a new task on the work queue.  An event
// will be signled to one of the waiting threads.
//
// -----------------------------------------------------------------
void ThreadPool::enqueueTask(std::shared_ptr<Tasks::Task> source)
{
	m_workQueue.enqueue(source);
	//
	// Notify a thread something was added to the queue
	std::unique_lock<std::mutex> lock(m_mutexWorkQueue);
	m_eventWorkQueue.notify_one();
}

// -----------------------------------------------------------------
//
// @details Shuts down all of the thread pool worker threads and removes
// the thread pool instance.
//
// -----------------------------------------------------------------
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
		std::unique_lock<std::mutex> lock(m_instance->m_mutexWorkQueue);
		m_instance->m_eventWorkQueue.notify_all();

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
