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
	bool finalizeTask(uint32_t id, bool forceRemove = false);

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

	std::priority_queue<std::shared_ptr<AssignedTask>, std::vector<std::shared_ptr<AssignedTask>>, AssignedTaskCompare> m_queueAssigned;
	std::unordered_map<uint32_t, std::shared_ptr<AssignedTask>> m_mapAssigned;
	std::recursive_mutex m_mutexAssigned;

	std::shared_ptr<std::thread> m_distributer;
	bool m_distributerDone;

	void distribute();
	void fillRequest(std::shared_ptr<Tasks::Task> task);
	void compactQueueAssigned();
	bool isQueueAssignedEmpty();
	void popQueueAssigned();
	bool mapAssignedContains(uint32_t id);
	boost::optional<std::shared_ptr<AssignedTask>> getQueueAssignedTop();
};

#endif // _TASKREQUESTQUEUE_HPP_
