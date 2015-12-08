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

#include "Task.hpp"

#include <limits>
#include <mutex>

// ------------------------------------------------------------------
//
// @details Base Task constructor that assigns a unique id to each
// task created.
//
// ------------------------------------------------------------------
Task::Task(std::function<void ()> onComplete) :
	m_onComplete(onComplete)
{
	static uint32_t currentId = 1;
	//
	// This lock ensures tasks are being assigned ids atomically
	static std::mutex myMutex;
	std::lock_guard<std::mutex> lock(myMutex);

	m_id = currentId++;

	//
	// Yes, I know this isn't a guarantee, but this is something I'm willing to live with.
	// If we go over 4 billion tasks and end up with a collision, that is when I'll
	// switch to something else, until then, this is just fine.
	if (currentId == std::numeric_limits<uint32_t>::max())
	{
		currentId = 1;
	}
}
