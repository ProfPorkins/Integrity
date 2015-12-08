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

#include <cstdint>
#include <functional>

// ------------------------------------------------------------------
//
// @details This is the base class from which all work items are derived.
//
// ------------------------------------------------------------------
class Task
{
public:
	explicit Task(std::function<void()> onComplete);
	virtual ~Task()					{}	// Virtual destructor to allow derived class destructors to correctly get called

	uint32_t getId() const			{ return m_id; }
	virtual void execute() = 0;
	void complete()					{ if (m_onComplete) { m_onComplete(); } }

protected:
	uint32_t m_id;

private:
	std::function<void ()> m_onComplete;
};

#endif // _TASK_HPP_
