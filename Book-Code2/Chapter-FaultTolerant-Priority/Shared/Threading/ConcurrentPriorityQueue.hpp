#ifndef _CONCURRENTPRIORITYQUEUE_HPP_
#define _CONCURRENTPRIORITYQUEUE_HPP_

#include <algorithm>
#include <mutex>
#include <vector>
#include <queue>

#include <boost/optional.hpp>

// ------------------------------------------------------------------
//
// @details This is a fairly simple concurrent priority_queue that allows 
// thread safe access.
//
// ------------------------------------------------------------------
template <typename T, typename P>
class ConcurrentPriorityQueue
{
public:
	// ------------------------------------------------------------------
	//
	// @details Enqueues a new item onto the queue
	//
	// ------------------------------------------------------------------
	void enqueue(const T& val)
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		m_queue.push(val);
	}

	// ------------------------------------------------------------------
	//
	// @details Attempts to dequeue the highest priority item.  If something
	// is available, it is returned, otherwise the optional is left empty.
	//
	// ------------------------------------------------------------------
	boost::optional<T> dequeue()
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		boost::optional<T> item;
		if (!m_queue.empty())
		{
			item = m_queue.top();
			m_queue.pop();
		}

		return item;
	}

private:
	std::priority_queue<T, std::vector<T>, P> m_queue;
	std::mutex m_mutex;
};

#endif // _CONCURRENTPRIORITYQUEUE_HPP_
