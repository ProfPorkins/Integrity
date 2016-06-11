#ifndef _THREADPOOL_HPP_
#define _THREADPOOL_HPP_

#include "ConcurrentQueue.hpp"
#include "Shared/Tasks/Task.hpp"
#include "WorkerThread.hpp"

#include <condition_variable>
#include <memory>
#include <mutex>
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

// -----------------------------------------------------------------
//
// @details Provides an implementation of a Thread Pool.  It is intended to
// be used to create & manage worker threads that handle all tasks throughout
// the system.
//
// -----------------------------------------------------------------
class ThreadPool
{
public:
	static std::shared_ptr<ThreadPool> instance();

	void initialize(boost::asio::io_service* ioService) { m_ioService = ioService; }
	void enqueueTask(std::shared_ptr<Tasks::Task> task);
	boost::asio::io_service* getIOService() { return m_ioService; }

	static void terminate();

protected:
	ThreadPool(uint16_t sizeInitial);

private:
	static std::shared_ptr<ThreadPool> m_instance;
	boost::asio::io_service* m_ioService;

	std::set<std::shared_ptr<WorkerThread>> m_threads;
	ConcurrentQueue<std::shared_ptr<Tasks::Task>> m_workQueue;
	std::condition_variable m_eventWorkQueue;
	std::mutex m_mutexWorkQueue;
};

#endif // _THREADPOOL_HPP_
