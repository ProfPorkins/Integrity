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

#ifndef _ASSIGNEDTASK_HPP_
#define _ASSIGNEDTASK_HPP_

#include "Shared/Tasks/Task.hpp"

#include <chrono>
#include <memory>

// -----------------------------------------------------------------
//
// @details This class holds the set of tasks that have been 
// assigned to a compute node and have not yet returned a result.  
// As tasks are added to this set, they are time-stamped with
// the current time and assigned a future deadline based upon the
// task timeout.  When a result is returned from a compute 
// node, the task is removed from this list and considered complete.
// When the deadline for a task has passed, it is removed from
// this list and returned back into the gloal work queue so that
// it can be re-assigned to a new compute node.
//
// TODO: This comment goes with the queue and the hash table.  This class
// is really just what is contained in those data structures.  But for
// now, this is the best place to have this comment.
//
// -----------------------------------------------------------------
class AssignedTask
{
public:
	AssignedTask(std::shared_ptr<Tasks::Task> task);

	std::shared_ptr<Tasks::Task> getTask() { return m_task; }
	void updateDeadline();
	std::chrono::time_point<std::chrono::high_resolution_clock> getDeadline() { return m_deadline; }

private:
	std::shared_ptr<Tasks::Task> m_task;
	std::chrono::time_point<std::chrono::high_resolution_clock> m_deadline;
};

// -----------------------------------------------------------------
//
// @details This class is used to compare assigned tasks for priority
// based upon their deadline.
//
// -----------------------------------------------------------------
struct AssignedTaskCompare
{
	bool operator()(const std::shared_ptr<AssignedTask>& lhs, const std::shared_ptr<AssignedTask>& rhs) const
	{
		//
		// Items with a closer dealine have higher priority.
		return (lhs->getDeadline() > rhs->getDeadline());
	}
};

#endif // _ASSIGNEDTASK_HPP_
