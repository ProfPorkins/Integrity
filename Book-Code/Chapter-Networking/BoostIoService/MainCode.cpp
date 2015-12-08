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

#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <mutex>

//
// Disable some compiler warnings that come from boost
#pragma warning(push)
#pragma warning(disable : 4267)
#pragma warning(disable : 4996)
#include <boost/asio.hpp>
#pragma warning(pop)

void basicIoService()
{
	boost::asio::io_service ioService;
	boost::asio::io_service::work work(ioService);

	std::thread ioThread = std::thread(
		[&ioService]()
		{
			ioService.run();
			std::cout << "Completed ioService." << std::endl;
		});

	std::cout << "Sleeping for a couple of seconds..." << std::endl;
	std::this_thread::sleep_for(std::chrono::milliseconds(2000));
	std::cout << "Stopping the ioService..." << std::endl;

	ioService.stop();
	ioThread.join();
}

void ioServiceQueue()
{
	boost::asio::io_service ioService;
	boost::asio::io_service::work work(ioService);

	std::cout << "main thread id: ";
	std::cout << std::this_thread::get_id() << std::endl;

	std::thread ioThread = std::thread(
		[&ioService]()
		{
			std::cout << "io_service thread id: ";
			std::cout << std::this_thread::get_id() << std::endl;
			ioService.run();
			std::cout << "io_service terminated" << std::endl;
		});

	auto notify =
		[]()
		{
			std::cout << "This post executed on: ";
			std::cout << std::this_thread::get_id() << std::endl;
		};

	ioService.post(notify);
	ioService.post(notify);

	ioService.stop();
	ioThread.join();
}

void threadedIoService()
{
	boost::asio::io_service ioService;
	boost::asio::io_service::work work(ioService);

	std::cout << "main thread id: ";
	std::cout << std::this_thread::get_id() << std::endl;

	std::vector<std::thread> threads;
	for (auto thread : IRange<uint8_t>(1, std::thread::hardware_concurrency()))
	{
		threads.push_back(std::thread(
			[&ioService]()
			{
				ioService.run();
			}));
	}

	auto notify =
		[]()
		{
			std::cout << "This post executed on: ";
			std::cout << std::this_thread::get_id() << std::endl;
		};

	ioService.post(notify);
	ioService.post(notify);

	ioService.stop();
	for (auto& thread : threads)
	{
		thread.join();
	}
}

int main()
{
	basicIoService();
	ioServiceQueue();
	threadedIoService();

	return 0;
}
