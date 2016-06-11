#include "FaultTolerantApp.hpp"

#include "Shared/Messages/NextPrime.hpp"
#include "Shared/Tasks/NextPrimeTask.hpp"
#include "Shared/TaskRequestQueue.hpp"

#include <iostream>
#include <chrono>
#include <iomanip>

// ------------------------------------------------------------------
//
// @details Initialize the Mandelbrot region, the mandelbrot rendering
// bitmap, and the message response command map.
//
// ------------------------------------------------------------------
FaultTolerantApp::FaultTolerantApp(HWND hwnd, uint16_t sizeX, uint16_t sizeY) :
	m_reportPrime(false)
{
}

// ------------------------------------------------------------------
//
// @details Create connections to each of the compute servers we'll be 
// using and begin waiting on a message from them.
//
// ------------------------------------------------------------------
bool FaultTolerantApp::initialize()
{
	m_ftFramework.initialize();

	m_ftFramework.registerHandler<Messages::NextPrimeResult>(
		Messages::Type::NextPrimeResult, 
		std::bind(&FaultTolerantApp::processNextPrimeResult, this, std::placeholders::_1));

	//
	// Generate the initial next prime request
	auto task = std::make_shared<Tasks::NextPrimeTask>(1, Tasks::Priority::High);
	TaskRequestQueue::instance()->enqueueTask(task);

	return true;
}

// ------------------------------------------------------------------
//
// @details Allows the application to be gracefully shut down.
//
// ------------------------------------------------------------------
void FaultTolerantApp::terminate()
{
	m_ftFramework.terminate();
}

// ------------------------------------------------------------------
//
// @details This is called as often as possible to render the latest
// and greatest prime number result.
//
// ------------------------------------------------------------------
void FaultTolerantApp::pulse()
{
	//
	// Render the most recently computed prime number
	if (m_reportPrime)
	{
		std::cout << "Next Prime: " << m_lastPrime << std::endl;
		m_reportPrime = false;
	}
}

// ------------------------------------------------------------------
//
// @details Receives the next prime result, displays it, and then 
// generates a request to compute the next prime number.
//
// ------------------------------------------------------------------
void FaultTolerantApp::processNextPrimeResult(std::shared_ptr<Messages::NextPrimeResult> taskResult)
{
	//
	// Generate a task request for the next prime
	auto task = std::make_shared<Tasks::NextPrimeTask>(taskResult->getNextPrime(), Tasks::Priority::High);
	TaskRequestQueue::instance()->enqueueTask(task);

	m_lastPrime = taskResult->getNextPrime();
	m_reportPrime = true;
}
