#include "WorkerThread.hpp"
#include "ThreadPool.hpp"
#include "Shared/TaskStatusTool.hpp"

// ------------------------------------------------------------------
//
// @details This constructor gets the underlying thread created
// along with saving references to the work queue, work queue event,
// and the number of available threads counter.
//
// ------------------------------------------------------------------
WorkerThread::WorkerThread(ConcurrentQueue<std::shared_ptr<Tasks::Task>>& workQueue, std::condition_variable& eventWorkQueue, std::mutex& mutexWorkQueue) :
	m_workQueue(workQueue),
	m_eventWorkQueue(eventWorkQueue),
	m_mutexWorkQueue(mutexWorkQueue),
	m_done(false),
	m_thread(nullptr)
{
	m_thread = new std::thread(&WorkerThread::run, this);
}

// ------------------------------------------------------------------
//
// @details This is the entry point method for the actual worker thread.  This
// method stays running until we are asked to voluntarily terminate.  The
// thread waits on a signal to check for something in the work queue.  If there
// is something in the queue, it goes to work.
//
// ------------------------------------------------------------------
void WorkerThread::run()
{
	while (!m_done)
	{
		boost::optional<std::shared_ptr<Tasks::Task>> task = m_workQueue.dequeue();
		if (task != boost::none)
		{
			task.get()->execute();
			task.get()->complete(*ThreadPool::instance()->getIOService());
			TaskStatusTool::instance()->removeTask(task.get()->getId());
		}
		else
		{
			std::unique_lock<std::mutex> lock(m_mutexWorkQueue);
			m_eventWorkQueue.wait(lock);
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
