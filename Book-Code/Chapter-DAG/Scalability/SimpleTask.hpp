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

#ifndef _SIMPLETASK_HPP_
#define _SIMPLETASK_HPP_

#include "Task.hpp"

#include <chrono>
#include <cstdint>
#include <iostream>
#include <thread>

// ------------------------------------------------------------------
//
// @details This is a task that doesn't really do anything, its whole
// purpose is to help demonstrate the DAG capabilities.
//
// ------------------------------------------------------------------
class SimpleTask : public Task
{
public:
	SimpleTask(uint16_t name) :
		Task(nullptr),
		m_name(name)
	{}

	virtual void execute()  override
	{
		//
		// Sleep for a little while to allow output to the console to
		// show the concurrency that takes place while executing these tasks.
		std::this_thread::sleep_for(std::chrono::milliseconds(3000));
		std::cout << m_name << std::endl;
	}

private:
	uint16_t m_name;
};

#endif // _SIMPLETASK_HPP_
