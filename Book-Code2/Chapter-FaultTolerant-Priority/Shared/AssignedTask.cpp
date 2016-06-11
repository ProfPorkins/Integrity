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
