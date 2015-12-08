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

#ifndef _TASK_HPP_
#define _TASK_HPP_

#include <functional>
#include <memory>

// ------------------------------------------------------------------
//
// @details This is the base class from which all tasks are derived.
//
// ------------------------------------------------------------------
class Task
{
public:
	enum class Priority : uint8_t
	{
		One = 1,
		Two = 2,
		Three = 3
	};

	explicit Task(std::function<void ()> onComplete, Priority priority = Priority::One) :
		m_onComplete(onComplete),
		m_priority(priority) 
	{
	}

	virtual ~Task()					{}	// Virtual destructor to allow derived class destructors to correctly get called

	virtual void execute() = 0;
	void complete()						{ if (m_onComplete) { m_onComplete(); } }

	Priority getPriority() const		{ return m_priority; }

private:
	Priority m_priority;
	std::function<void ()> m_onComplete;
};

// ------------------------------------------------------------------
//
// @details This class is used to compare tasks for priority
//
// ------------------------------------------------------------------
class TaskCompare : public std::binary_function<const std::shared_ptr<const Task>&, const std::shared_ptr<const Task>&, bool>
{
public:
	bool operator()(const std::shared_ptr<const Task>& lhs, const std::shared_ptr<const Task>& rhs) const
	{
		return (lhs->getPriority() < rhs->getPriority());
	}
};

#endif // _TASK_HPP_
