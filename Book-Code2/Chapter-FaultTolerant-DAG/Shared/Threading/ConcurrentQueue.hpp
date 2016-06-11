#ifndef _CONCURRENTQUEUE_HPP_
#define _CONCURRENTQUEUE_HPP_

#include <mutex>
#include <queue>

#include <boost/optional.hpp>

// ------------------------------------------------------------------
//
// @details This is a fairly simple concurrent queue that allows thread
// safe access.
//
// ------------------------------------------------------------------
template <typename T>
class ConcurrentQueue
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
	// @details Attempts to dequeue an item from the queue.  If there is an 
	// item it is returned, otherwise the optional is left empty.
	//
	// ------------------------------------------------------------------
	boost::optional<T> dequeue()
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		boost::optional<T> item = boost::none;
		if (!m_queue.empty())
		{
			item = m_queue.front();
			m_queue.pop();
		}

		return item;
	}
	
private:
	std::queue<T> m_queue;
	std::mutex m_mutex;
};

#endif // _CONCURRENTQUEUE_HPP_
