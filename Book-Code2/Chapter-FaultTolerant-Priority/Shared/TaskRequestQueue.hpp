#ifndef _TASKREQUESTQUEUE_HPP_
#define _TASKREQUESTQUEUE_HPP_

#include "AssignedTask.hpp"
#include "ServerSet.hpp"
#include "Shared/Tasks/Task.hpp"
#include "Shared/Threading/ConcurrentPriorityQueue.hpp"

#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <unordered_map>
#include <vector>

#pragma warning(push)
#pragma warning(disable : 4390)
#include <boost/heap/binomial_heap.hpp>
#pragma warning(pop)

// ------------------------------------------------------------------
//
// @details The TaskRequestQueue is used to collect and distribute work
// items to the distributed computing resources.  This queue is expected
// to exist on a single client computer, with any number of connected
// compute servers.  Compute servers indicate availability by sending a
// TaskRequest notification to the client, which is then added to this
// queue.  The client adds new tasks to this queue and those work
// items are distributed to compute servers by filling any available
// work requeusts.
//
// ------------------------------------------------------------------
class TaskRequestQueue
{
public:
	static std::shared_ptr<TaskRequestQueue> instance();

	void initialize(boost::asio::io_service* ioService, ServerSet* servers);
	void terminate();
	void enqueueRequest(ServerID_t request);

	void enqueueTask(std::shared_ptr<Tasks::Task> task);

	void touchTask(uint64_t taskId);
	bool finalizeTask(uint64_t id, bool dagRemove, bool forceRemove);

protected:
	TaskRequestQueue();

private:
	static std::shared_ptr<TaskRequestQueue> m_instance;
	boost::asio::io_service* m_ioService;
	ServerSet* m_servers;

	std::queue<ServerID_t> m_queueRequest;
	std::mutex m_mutexRequest;
	std::condition_variable m_eventRequest;
	std::mutex m_mutexEventRequest;

	ConcurrentPriorityQueue<std::shared_ptr<Tasks::Task>, TaskCompare> m_queueTasks;
	std::condition_variable m_eventTask;
	std::mutex m_mutexEventTask;

	typedef boost::heap::binomial_heap<std::shared_ptr<AssignedTask>, boost::heap::compare<AssignedTaskCompare>> PriorityQueue;
	PriorityQueue m_queueAssigned;
	std::unordered_map<uint64_t, std::shared_ptr<AssignedTask>> m_mapAssigned;
	std::unordered_map<uint64_t, PriorityQueue::handle_type> m_pqHandles;
	std::recursive_mutex m_mutexAssigned;

	std::shared_ptr<std::thread> m_distributer;
	bool m_distributerDone;

	void distribute();
	void fillRequest(std::shared_ptr<Tasks::Task> task);
	void compactQueueAssigned();
	bool isQueueAssignedEmpty();
	void popQueueAssigned();
	bool mapAssignedContains(uint64_t id);
	boost::optional<std::shared_ptr<AssignedTask>> getQueueAssignedTop();
};

#endif // _TASKREQUESTQUEUE_HPP_
