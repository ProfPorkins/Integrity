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

#include "AssignedTask.hpp"

#include <chrono>

// -----------------------------------------------------------------
//
// @details This constructor prepares the deadline for when the task
// is expected to complete.
//
// -----------------------------------------------------------------
AssignedTask::AssignedTask(std::shared_ptr<Tasks::Task> task) :
	m_task(task)
{
	//
	// Set the initial deadline for when we expect to receive the next
	// status for this task.
	updateDeadline();
}

// -----------------------------------------------------------------
//
// @details Sets the deadline for when the task is expected to report
// in with a status message.
//
// -----------------------------------------------------------------
void AssignedTask::updateDeadline()
{
	//
	// By definition we set the new deadline to 2 seconds from now.
	auto now = std::chrono::high_resolution_clock::now();
	m_deadline = now + std::chrono::milliseconds(3000);
}
