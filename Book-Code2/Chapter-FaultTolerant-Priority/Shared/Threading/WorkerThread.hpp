#ifndef _WORKERTHREAD_HPP_
#define _WORKERTHREAD_HPP_

#include "ConcurrentQueue.hpp"
#include "Shared/Tasks/Task.hpp"

#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>

// -----------------------------------------------------------------
//
// @details This class provides the implementation for worker threads
// throughout the system.  It provides the key functionality that knows
// how to effeciently wait on a work queue, then, as tasks become
// available, it grabs the next one and works on it.
//
// -----------------------------------------------------------------
class WorkerThread
{
public:
	WorkerThread(ConcurrentQueue<std::shared_ptr<Tasks::Task>>& workQueue, std::condition_variable& eventWorkQueue, std::mutex& mutexWorkQueue);

	void run();
	void terminate();
	void join();

private:
	std::thread* m_thread;	// Have to manage the memory ourselves, do NOT delete when finished!
	bool m_done;

	ConcurrentQueue<std::shared_ptr<Tasks::Task>>& m_workQueue;
	std::condition_variable& m_eventWorkQueue;
	std::mutex& m_mutexWorkQueue;
};

#endif // _WORKERTHREAD_HPP_
