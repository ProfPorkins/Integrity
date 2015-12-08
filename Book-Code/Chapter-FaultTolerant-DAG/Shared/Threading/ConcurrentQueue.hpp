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

		boost::optional<T> item = nullptr;
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
