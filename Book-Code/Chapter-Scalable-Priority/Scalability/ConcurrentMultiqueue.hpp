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

#ifndef _CONCURRENTMULTIQUEUE_HPP_
#define _CONCURRENTMULTIQUEUE_HPP_

#include <algorithm>
#include <mutex>
#include <set>

// ------------------------------------------------------------------
//
// @details This is a fairly simple concurrent queue that allows thread
// safe access.  The queue is also a priority queue allowing the highest
// priority item to be dequeued, or by selecting a priority and dequeuing
// items of the specified priority in the order they were enqueued.
//
// ------------------------------------------------------------------
template <typename T, typename P, typename C>
class ConcurrentMultiqueue
{
public:
	// ------------------------------------------------------------------
	//
	// @details Enqueues a new item onto the queue
	//
	// ------------------------------------------------------------------
	void enqueue(const T& item)
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		m_queue.insert(item);
	}

	// ------------------------------------------------------------------
	//
	// @details Attempts to dequeue an item from the queue based upon the
	// specified priority.  If there is an item of that priority available
	// it is returned by reference.  True is returned if successful, false otherwise.
	//
	// ------------------------------------------------------------------
	bool dequeue(P priority, T& item)
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		bool success = false;

		//
		// 'item' ends up not actually being used, but the algorithm requires some value is passed in
		auto itr = std::lower_bound(m_queue.begin(), m_queue.end(), item,
			[priority](const T& value1, const T& value2)
			{
				//
				// We are saying we know something about the types involved with this
				// queue...so sue me!
				return value1->getPriority() < priority;
			});

		if (itr != m_queue.end())
		{
			item = *itr;
			m_queue.erase(itr);

			success = true;
		}

		return success;
	}
	
	// ------------------------------------------------------------------
	//
	// @details Attempts to dequeue the highest priority item.  If something
	// is available, it is returned by reference.
	// True is returned if successful, false otherwise.
	//
	// ------------------------------------------------------------------
	bool dequeue(T& item)
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		bool success = false;

		auto itr = m_queue.begin();
		if (itr != m_queue.end())
		{
			item = *itr;
			m_queue.erase(itr);

			success = true;
		}

		return success;
	}

private:
	std::multiset<T, C> m_queue;	// Using an ordered set to ensure items are removed in the order they were inserted
	std::mutex m_mutex;
};

#endif // _CONCURRENTMULTIQUEUE_HPP_
