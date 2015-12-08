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

#include "IRange.hpp"

#include <cstdint>
#include <iostream>
#include <thread>

void simpleThread()
{
	auto report = []()
	{
		std::cout << "This executed from thread: ";
		std::cout << std::this_thread::get_id();
		std::cout << std::endl;
	};

	std::thread myThread(report);
	myThread.join();

	report();
}

void sleepingThread()
{
	std::thread myThread(
		[]()
		{
			std::cout << "Going to sleep for two seconds...";
			std::this_thread::sleep_for(std::chrono::milliseconds(2000));
			std::cout << "awakened" << std::endl;
		});

	myThread.join();
}

void howManyCores()
{
	auto coreCount = std::thread::hardware_concurrency();

	std::cout << "This system has " << coreCount;
	std::cout << " CPU cores" << std::endl;
}

void timeFibonacci()
{
	std::function<uint32_t (uint16_t )> fib;
	fib = [&fib](uint16_t n)->uint32_t 
	{
		if (n == 0) return 1;
		if (n == 1) return 1;

		return fib(n - 1) + fib(n - 2);
	};

	auto start = std::chrono::system_clock::now();

	for (auto n: IRange<uint16_t>(0, 10))
	{
		std::cout << "Fibonacci of " << n << " is ";
		std::cout << fib(n) << std::endl;
	}

	auto end = std::chrono::system_clock::now();

	std::chrono::nanoseconds difference = end - start;

	std::cout << "Time to compute is: " << difference.count();
	std::cout << " nanoseconds" << std::endl;
}

int main()
{
	std::cout << "-- simpleThread --" << std::endl;
	simpleThread();

	std::cout << std::endl << "-- sleepingThread --" << std::endl;
	sleepingThread();

	std::cout << std::endl << "-- howManyCores --" << std::endl;
	howManyCores();

	std::cout << std::endl << "-- timeFibonacci --" << std::endl;
	timeFibonacci();

	return 0;
}
