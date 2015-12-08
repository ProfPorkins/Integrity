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

#include "ScalabilityApp.hpp"

#include "NextPrimeTask.hpp"
#include "SimpleTask.hpp"
#include "ThreadPool.hpp"

#include <iostream>

// ------------------------------------------------------------------
//
// @details Initialize the Mandelbrot region, the mandelbrot rendering
// bitmap, and kick off the generation of prime numbers.
//
// ------------------------------------------------------------------
ScalabilityApp::ScalabilityApp(HWND hwnd, uint16_t sizeX, uint16_t sizeY) :
	m_currentPrime(1),
	m_reportPrime(false),
	m_lastPrime(m_currentPrime)
{
	m_mandelbrot = std::make_shared<Mandelbrot>(hwnd, sizeX, sizeY);

	//
	// Start computing prime numbers
	//startNextPrime();

	startChapterDemo();
}

// ------------------------------------------------------------------
//
// @details This is called as often as possible to render the Mandelbrot
// set.  If any change in the view has been requested, a new task is
// started that will cause the new view to be rendered.
//
// ------------------------------------------------------------------
void ScalabilityApp::pulse()
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
// @details Based upon the currently known largest prime number,
// generate the next prime in the sequence.
//
// ------------------------------------------------------------------
void ScalabilityApp::startNextPrime()
{
	m_lastPrime = m_currentPrime;
	m_reportPrime = true;

	auto task = std::make_shared<NextPrimeTask>(
		m_lastPrime,
		m_currentPrime,
		[this]() { startNextPrime(); });

	ThreadPool::instance()->enqueueTask(task);
}

// ------------------------------------------------------------------
//
// @details This method prepares tasks based upon the DAG (Figure 6.1) 
// from the book.
//
// ------------------------------------------------------------------
void ScalabilityApp::startChapterDemo()
{
	ThreadPool::instance()->beginGroup();

	auto task1 = std::make_shared<SimpleTask>(1);
	auto task2 = std::make_shared<SimpleTask>(2);
	auto task3 = std::make_shared<SimpleTask>(3);
	auto task4 = std::make_shared<SimpleTask>(4);
	auto task5 = std::make_shared<SimpleTask>(5);
	auto task6 = std::make_shared<SimpleTask>(6);

	ThreadPool::instance()->enqueueTask(task1, task4);
	ThreadPool::instance()->enqueueTask(task2, task4);
	ThreadPool::instance()->enqueueTask(task2, task5);
	ThreadPool::instance()->enqueueTask(task3, task4);
	ThreadPool::instance()->enqueueTask(task4, task6);
	ThreadPool::instance()->enqueueTask(task5, task6);
	
	ThreadPool::instance()->endGroup();
}
