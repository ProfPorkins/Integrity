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

#include "FaultTolerantApp.hpp"

#include "Shared/Messages/DAGExample.hpp"
#include "Shared/Messages/NextPrime.hpp"
#include "Shared/Tasks/DAGExampleTask.hpp"
#include "Shared/Tasks/NextPrimeTask.hpp"
#include "Shared/TaskRequestQueue.hpp"

#include <iostream>
#include <chrono>
#include <iomanip>

namespace 
{
	// ------------------------------------------------------------------
	//
	// @details This method prepares tasks based upon the DAG (Figure 6.1) 
	// from the book.
	//
	// ------------------------------------------------------------------
	void startChapterDemo()
	{
		TaskRequestQueue::instance()->beginGroup();

		auto task1 = std::make_shared<Tasks::DAGExampleTask>("1");
		auto task2 = std::make_shared<Tasks::DAGExampleTask>("2");
		auto task3 = std::make_shared<Tasks::DAGExampleTask>("3");
		auto task4 = std::make_shared<Tasks::DAGExampleTask>("4");
		auto task5 = std::make_shared<Tasks::DAGExampleTask>("5");
		auto task6 = std::make_shared<Tasks::DAGExampleTask>("6");
		auto task7 = std::make_shared<Tasks::DAGExampleTask>("7");
		auto task8 = std::make_shared<Tasks::DAGExampleTask>("8");
		auto task9 = std::make_shared<Tasks::DAGExampleTask>("9");
		auto task10 = std::make_shared<Tasks::DAGExampleTask>("10");
		auto task11 = std::make_shared<Tasks::DAGExampleTask>("11");
		auto task12 = std::make_shared<Tasks::DAGExampleTask>("12");
		auto task13 = std::make_shared<Tasks::DAGExampleTask>("13");
		auto task14 = std::make_shared<Tasks::DAGExampleTask>("14");
		auto task15 = std::make_shared<Tasks::DAGExampleTask>("15");

		auto task16 = std::make_shared<Tasks::DAGExampleTask>("16");
		auto task17 = std::make_shared<Tasks::DAGExampleTask>("17");
		auto task18 = std::make_shared<Tasks::DAGExampleTask>("18");
		auto task19 = std::make_shared<Tasks::DAGExampleTask>("19");
		auto task20 = std::make_shared<Tasks::DAGExampleTask>("20");
		auto task21 = std::make_shared<Tasks::DAGExampleTask>("21");
		auto task22 = std::make_shared<Tasks::DAGExampleTask>("22");
		auto task23 = std::make_shared<Tasks::DAGExampleTask>("23");
		auto task24 = std::make_shared<Tasks::DAGExampleTask>("24");
		auto task25 = std::make_shared<Tasks::DAGExampleTask>("25");
		auto task26 = std::make_shared<Tasks::DAGExampleTask>("26");
		auto task27 = std::make_shared<Tasks::DAGExampleTask>("27");
		auto task28 = std::make_shared<Tasks::DAGExampleTask>("28");
		auto task29 = std::make_shared<Tasks::DAGExampleTask>("29");

		TaskRequestQueue::instance()->enqueueTask(task2, task1);
		TaskRequestQueue::instance()->enqueueTask(task3, task1);
		TaskRequestQueue::instance()->enqueueTask(task4, task2);
		TaskRequestQueue::instance()->enqueueTask(task5, task2);
		TaskRequestQueue::instance()->enqueueTask(task6, task3);
		TaskRequestQueue::instance()->enqueueTask(task7, task3);
		TaskRequestQueue::instance()->enqueueTask(task8, task4);
		TaskRequestQueue::instance()->enqueueTask(task9, task4);
		TaskRequestQueue::instance()->enqueueTask(task10, task5);
		TaskRequestQueue::instance()->enqueueTask(task11, task5);
		TaskRequestQueue::instance()->enqueueTask(task12, task6);
		TaskRequestQueue::instance()->enqueueTask(task13, task6);
		TaskRequestQueue::instance()->enqueueTask(task14, task7);
		TaskRequestQueue::instance()->enqueueTask(task15, task7);

		TaskRequestQueue::instance()->enqueueTask(task1, task16);
		TaskRequestQueue::instance()->enqueueTask(task1, task17);
		TaskRequestQueue::instance()->enqueueTask(task16, task18);
		TaskRequestQueue::instance()->enqueueTask(task16, task19);
		TaskRequestQueue::instance()->enqueueTask(task17, task20);
		TaskRequestQueue::instance()->enqueueTask(task17, task21);
		TaskRequestQueue::instance()->enqueueTask(task18, task22);
		TaskRequestQueue::instance()->enqueueTask(task18, task23);
		TaskRequestQueue::instance()->enqueueTask(task19, task24);
		TaskRequestQueue::instance()->enqueueTask(task19, task25);
		TaskRequestQueue::instance()->enqueueTask(task20, task26);
		TaskRequestQueue::instance()->enqueueTask(task20, task27);
		TaskRequestQueue::instance()->enqueueTask(task21, task28);
		TaskRequestQueue::instance()->enqueueTask(task21, task29);

		TaskRequestQueue::instance()->endGroup();
	}
}

// ------------------------------------------------------------------
//
// @details Initialize the Mandelbrot region, the mandelbrot rendering
// bitmap, and the message response command map.
//
// ------------------------------------------------------------------
FaultTolerantApp::FaultTolerantApp(HWND hwnd, uint16_t sizeX, uint16_t sizeY) :
	m_reportPrime(false)
{
	//
	// Get the mandelbrot code created and initialized
	m_mandelbrot = std::make_shared<Mandelbrot>(hwnd, sizeX, sizeY);
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

	//
	// Register our message handlers
	m_ftFramework.registerHandler<Messages::MandelResult>(
		Messages::Type::MandelResult, 
		std::bind(&FaultTolerantApp::processMandelResult, this, std::placeholders::_1));

	m_ftFramework.registerHandler<Messages::MandelFinishedResult>(
		Messages::Type::MandelFinishedResult, 
		std::bind(&FaultTolerantApp::processMandelFinishedResult, this, std::placeholders::_1));

	m_ftFramework.registerHandler<Messages::NextPrimeResult>(
		Messages::Type::NextPrimeResult, 
		std::bind(&FaultTolerantApp::processNextPrimeResult, this, std::placeholders::_1));

	m_ftFramework.registerHandler<Messages::DAGExampleResult>(
		Messages::Type::DAGExampleResult, 
		std::bind(&FaultTolerantApp::processDAGExampleResult, this, std::placeholders::_1));

	//
	// Generate the initial next prime request
	auto task = std::make_shared<Tasks::NextPrimeTask>(1);
	TaskRequestQueue::instance()->enqueueTask(task);

	//
	// Initiate the complex DAG demo
	//startChapterDemo();

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
// @details This is called as often as possible to render the Mandelbrot
// set.  If any change in the view has been requested, a new task is
// started that will cause the new view to be rendered.
//
// ------------------------------------------------------------------
void FaultTolerantApp::pulse()
{
	m_mandelbrot->update();

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
// @details Places the pixels for this result copied into the displayable image.
//
// ------------------------------------------------------------------
void FaultTolerantApp::processMandelResult(std::shared_ptr<Messages::MandelResult> taskResult)
{
	m_mandelbrot->processMandelResult(*taskResult);
}

// ------------------------------------------------------------------
//
// @details Finishes reading the mandel finished result and invokes
// the appropriate handler in the Mandelbrot class to signal it is
// now complete.
//
// ------------------------------------------------------------------
void FaultTolerantApp::processMandelFinishedResult(std::shared_ptr<Messages::MandelFinishedResult> taskResult)
{
	m_mandelbrot->processMandelFinishedResult(*taskResult);
}

// ------------------------------------------------------------------
//
// @details Finishes reading the next prime result, displays it, and
// then generates a request to compute the next prime number.
//
// ------------------------------------------------------------------
void FaultTolerantApp::processNextPrimeResult(std::shared_ptr<Messages::NextPrimeResult> taskResult)
{
	//std::chrono::time_point<std::chrono::high_resolution_clock, std::chrono::nanoseconds> now = std::chrono::high_resolution_clock ::now();
	//std::cout << "Received Result" << std::fixed << std::setprecision(10) << (now.time_since_epoch().count() / 1000000000.0) << std::endl;

	//
	// Generate a task request for the next prime
	auto task = std::make_shared<Tasks::NextPrimeTask>(taskResult->getNextPrime());
	TaskRequestQueue::instance()->enqueueTask(task);

	m_lastPrime = taskResult->getNextPrime();
	m_reportPrime = true;
}

// ------------------------------------------------------------------
//
// @details Finishes reading the DAG Example task and reports its
// name to the console.
//
// ------------------------------------------------------------------
void FaultTolerantApp::processDAGExampleResult(std::shared_ptr<Messages::DAGExampleResult> taskResult)
{
	std::cout << "DAG Example Task: " << taskResult->getName() << std::endl;
}
