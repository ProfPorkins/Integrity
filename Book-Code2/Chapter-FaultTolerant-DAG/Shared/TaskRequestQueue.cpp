#include "TaskRequestQueue.hpp"

#include <chrono>
#include <iostream>
#include <iomanip>

std::shared_ptr<TaskRequestQueue> TaskRequestQueue::m_instance = nullptr;

// ------------------------------------------------------------------
//
// @details Protected constructor used to initialize the single
// instance of the queue/distributer.
//
// ------------------------------------------------------------------
TaskRequestQueue::TaskRequestQueue() :
m_distributerDone(false)
{
}

// ------------------------------------------------------------------
//
// @details This is the Singleton ::instance accessor method.  The first
// time this is called, a new thread is created that is used to distribute
// any tasks out to the waiting compute servers.
//
// ------------------------------------------------------------------
std::shared_ptr<TaskRequestQueue> TaskRequestQueue::instance()
{
	if (m_instance)		return m_instance;

	//
	// Can't use std::make_shared because of the protected constructor
	m_instance = std::shared_ptr<TaskRequestQueue>(new TaskRequestQueue());

	//
	// Create a thread that listens to the work queue and distributes those tasks to pending task requests
	m_instance->m_distributer = std::make_shared<std::thread>(std::bind(&TaskRequestQueue::distribute, m_instance));
	return m_instance;
}

// ------------------------------------------------------------------
//
// @details This allows the singleton to be initialized with the set
// of compute servers.
//
// ------------------------------------------------------------------
void TaskRequestQueue::initialize(boost::asio::io_service* ioService, ServerSet* servers)
{
	m_ioService = ioService;
	m_servers = servers;
}

// ------------------------------------------------------------------
//
// @details Tells the work distributer to stop and blocks until that
// thread completes.
//
// ------------------------------------------------------------------
void TaskRequestQueue::terminate()
{
	m_distributerDone = true;
	{
		std::unique_lock<std::mutex> lock(m_mutexEventTask);
		m_eventTask.notify_all();
	}
	{
		std::unique_lock<std::mutex> lock(m_mutexEventRequest);
		m_eventRequest.notify_all();
	}

	m_distributer->join();
}

// ------------------------------------------------------------------
//
// @details This places a new task request on the request queue.  An event
// will be signaled to one of the waiting threads.
//
// ------------------------------------------------------------------
void TaskRequestQueue::enqueueRequest(ServerID_t request)
{
	std::unique_lock<std::mutex> lock(m_mutexRequest);

	m_queueRequest.push(request);
	m_eventRequest.notify_one();
}

// ------------------------------------------------------------------
//
// @details This places a new task on the working queue.  An event
// will be signled to one of the waiting threads.
//
// ------------------------------------------------------------------
void TaskRequestQueue::enqueueTask(std::shared_ptr<Tasks::Task> source)
{
	//std::chrono::time_point<std::chrono::high_resolution_clock, std::chrono::nanoseconds> now = std::chrono::high_resolution_clock::now();
	//std::cout << "Enqueued Task" << std::fixed << std::setprecision(10) << (now.time_since_epoch().count() / 1000000000.0) << std::endl;

	m_queueTasks.addNode(source);
	std::unique_lock<std::mutex> lock(m_mutexEventTask);
	m_eventTask.notify_all();
}

// ------------------------------------------------------------------
//
// @details This places a new task, along with identifying a task that
// can not be computed (the dependent) until the source task has finished.
//
// ------------------------------------------------------------------
void TaskRequestQueue::enqueueTask(std::shared_ptr<Tasks::Task> source, std::shared_ptr<Tasks::Task> dependent)
{
	m_queueTasks.addEdge(source, dependent);
	std::unique_lock<std::mutex> lock(m_mutexEventTask);
	m_eventTask.notify_all();
}

// ------------------------------------------------------------------
//
// @details This is called when a status message for a task is recieved.
// The task tracking is updated with the time the status message was
// received.
//
// ------------------------------------------------------------------
void TaskRequestQueue::touchTask(uint64_t taskId)
{
	std::lock_guard<std::recursive_mutex> lock(m_mutexAssigned);

	auto task = m_mapAssigned.find(taskId);
	if (task != m_mapAssigned.end())
	{
		task->second->updateDeadline();
		//
		// With a new deadline, have to update the priority queue
		auto handle = m_pqHandles[taskId];
		m_queueAssigned.decrease(handle, task->second);
	}
}

// ------------------------------------------------------------------
//
// @details This is used to inform that the result for this task
// has been returned and that it can be removed from the list of outstanding
// results.
//
// ------------------------------------------------------------------
bool TaskRequestQueue::finalizeTask(uint64_t id, bool dagRemove, bool forceRemove)
{
	std::lock_guard<std::recursive_mutex> lock(m_mutexAssigned);
	auto removed = bool{ false };

	auto it = m_mapAssigned.find(id);
	if (it != m_mapAssigned.end())
	{
		//
		// Inform the DAG this task is done and can be removed
		if (dagRemove)
		{
			m_queueTasks.finalize(it->second->getTask());
		}

		//
		// If it came in after its deadline, don't erase from the assigned hash
		// table because we are going to ignore the result from anything that
		// showed up late....we want to treat it as if it failed completely.
		std::chrono::time_point<std::chrono::high_resolution_clock> now = std::chrono::high_resolution_clock::now();
		if (it->second->getDeadline() >= now || forceRemove)
		{
			m_pqHandles.erase(id);
			m_mapAssigned.erase(id);
			removed = true;
		}
	}
	else // Debugging code
	{
		std::cout << "could not find it: " << id << std::endl;
	}

	//
	// Because there might be dependent tasks that are now freed up, notify the event
	// to release any dependent tasks.  This should be a notify_all because more than
	// one task may become freed for work.
	std::unique_lock<std::mutex> lockTask(m_mutexEventTask);
	m_eventTask.notify_all();

	return removed;
}

// ------------------------------------------------------------------
//
// @details This is the entry point method for the actual work distribution
// thread.  This method stays running until we are asked to voluntarily terminate.  
// The thread waits on a signal to check for something in the work queue.  If there
// is something in the work queue and an available task request, work is sent to
// the server with an open request.
//
// ------------------------------------------------------------------
void TaskRequestQueue::distribute()
{
	while (!m_distributerDone)
	{
		auto keepTrying= bool{ true };
		while (keepTrying)
		{
			auto distributed = bool{ false };
			//
			// Clean out items from the assigned queue that have been finalized
			compactQueueAssigned();

			//
			// Step 1: Look at the pending result queue.  If the top item is past its deadline, take it as
			// a new task.
			if (!isQueueAssignedEmpty())
			{
				auto top = getQueueAssignedTop();
				if (top)
				{
					auto now = std::chrono::high_resolution_clock::now();
					if (top.get()->getDeadline() <= now)
					{
						std::cout << "retrying some work..." << std::endl;
						// clang on MacOS doesn't have to_time_t, so commenting out for now
						//std::cout << "Deadline : " << std::chrono::high_resolution_clock::to_time_t(top.get()->getDeadline()) << std::endl;
						//std::cout << "Now      : " << std::chrono::high_resolution_clock::to_time_t(now) << std::endl;
						std::shared_ptr<Tasks::Task> task = top.get()->getTask();
						//
						// This task will be added back into both the assigned queue and the assigned map, so remove it
						// from both to prevent duplication.
						popQueueAssigned();
						finalizeTask(task->getId(), false, true);

						fillRequest(task);
						distributed = true;
					}
				}
			}
			//
			// Step 2: Look at the new work queue and pull something from there if possible
			if (!distributed)
			{
				auto task = m_queueTasks.dequeue();
				if (task != boost::none)
				{
					fillRequest(task.get());
					distributed = true;
				}
				else
				{
					keepTrying = false;
				}
			}
		}
		if (!m_distributerDone)
		{
			//
			// Doing a wait_for here because it is possible for there to be only a single outstanding
			// request and if it fails, we need a way to come out of the wait and check to see if a failure
			// has occured and deal with it.  The wait time can be very small because this is only ever going
			// to consume a single CPU core at most.
			std::unique_lock<std::mutex> lock(m_mutexEventTask);
			m_eventTask.wait_for(lock, std::chrono::milliseconds(100));
		}
	}
}

// ------------------------------------------------------------------
//
// @details This method is used to send a task to a compute server
// that has indicated (via a task request) that it is available to
// do some work.
//
// ------------------------------------------------------------------
void TaskRequestQueue::fillRequest(std::shared_ptr<Tasks::Task> task)
{
	auto removed = m_servers->removeDisconnected();
	//
	// This is slow, but has to be done.  If any servers where removed, have to go through the
	// queue and remove any requests from those servers.  If those requests aren't removed it
	// causes a real performance delay because attempts to match requests from those servers
	// are still performed.
	if (!removed.empty())
	{
		std::lock_guard<std::mutex> lockRequest(m_mutexRequest);
		std::queue<ServerID_t> validRequests;
		while (!m_queueRequest.empty())
		{
			auto serverId = m_queueRequest.front();
			m_queueRequest.pop();
			if (removed.find(serverId) == removed.end())
			{
				validRequests.push(serverId);
			}
		}

		m_queueRequest = std::move(validRequests);
	}

	auto serverId = ServerID_t{ 0 };
	auto done = bool{ false };
	while (!done)
	{
		//
		// Try to get a matching request
		{
			std::lock_guard<std::mutex> lockRequest(m_mutexRequest);
			if (!m_queueRequest.empty())
			{
				serverId = m_queueRequest.front();
				m_queueRequest.pop();
				done = true;
			}
		}
		if (!done)
		{
			std::unique_lock<std::mutex> lock(m_mutexEventRequest);
			m_eventRequest.wait(lock);
		}
	}

	m_ioService->post(
		[this, serverId, task]()
	{
		//
		// Waiting to add it to the assigned queue until it actually gets processed by the io_service
		// thread, thinking that gives a more accurate time through the system.  Although, it can
		// be argued the correct time is when it is posted, because the time spent waiting in the
		// io_service queue is real time that counts against the deadline.
		{
			std::lock_guard<std::recursive_mutex> lock(m_mutexAssigned);
			auto assigned = std::make_shared<AssignedTask>(task);
			auto handle = m_queueAssigned.push(assigned);
			m_pqHandles[task->getId()] = handle;
			m_mapAssigned[task->getId()] = assigned;
		}

		//std::chrono::time_point<std::chrono::high_resolution_clock, std::chrono::nanoseconds> now = std::chrono::high_resolution_clock::now();
		//std::cout << "Sending Task" << std::fixed << std::setprecision(10) << (now.time_since_epoch().count() / 1000000000.0) << std::endl;

		task->send(m_servers->get(serverId)->socket, *m_servers->get(serverId)->strand);
	});
}

// ------------------------------------------------------------------
//
// @details Remove things from the queue that are not found in the 
// assigned hash table.  Items found in the assigned queue and not 
// in the assigned hash table are those that have a result returned
// and finalized.  Therefore, we want to ignore them.
//
// ------------------------------------------------------------------
void TaskRequestQueue::compactQueueAssigned()
{
	std::lock_guard<std::recursive_mutex> lock(m_mutexAssigned);

	auto done = bool{ false };
	while (!isQueueAssignedEmpty() && !done)
	{
		auto top = getQueueAssignedTop();
		if (top)
		{
			if (!mapAssignedContains(top.get()->getTask()->getId()))
			{
				popQueueAssigned();
			}
			else
			{
				done = true;
			}
		}
	}
}

// ------------------------------------------------------------------
//
// @details Returns true if the assigned work queue is empty, false
// otherwise.
//
// ------------------------------------------------------------------
bool TaskRequestQueue::isQueueAssignedEmpty()
{
	std::lock_guard<std::recursive_mutex> lock(m_mutexAssigned);

	return m_queueAssigned.empty();
}

// ------------------------------------------------------------------
//
// @details Removes the top item from the assigned work queue.
//
// ------------------------------------------------------------------
void TaskRequestQueue::popQueueAssigned()
{
	std::lock_guard<std::recursive_mutex> lock(m_mutexAssigned);

	m_queueAssigned.pop();
}

// ------------------------------------------------------------------
//
// @details Checks to see if a task id exists in the assigned hash table.
//
// ------------------------------------------------------------------
bool TaskRequestQueue::mapAssignedContains(uint64_t id)
{
	std::lock_guard<std::recursive_mutex> lock(m_mutexAssigned);

	return !(m_mapAssigned.find(id) == m_mapAssigned.end());
}

// ------------------------------------------------------------------
//
// @details Returns the top item from the assigned queue.  The item
// is not removed from the queue.  If there are no items in the queue
// the return value is a nullptr.
//
// ------------------------------------------------------------------
boost::optional<std::shared_ptr<AssignedTask>> TaskRequestQueue::getQueueAssignedTop()
{
	boost::optional<std::shared_ptr<AssignedTask>> result;	// defaults to uninitialized
	std::lock_guard<std::recursive_mutex> lock(m_mutexAssigned);

	if (!m_queueAssigned.empty())
	{
		result = m_queueAssigned.top();
	}

	return result;
}
