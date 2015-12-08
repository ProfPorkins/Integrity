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
#include <string>
#include <thread>
#include <vector>

//
// Disable some compiler warnings that come from boost
#pragma warning(push)
#pragma warning(disable : 4267)
#pragma warning(disable : 4996)
#include <boost/asio.hpp>
#pragma warning(pop)

class Countdown
{
public:
	Countdown(boost::asio::io_service& ioService, std::string name, uint16_t startCount) :
		m_strand(ioService),
		m_name(name),
		m_count(startCount)
	{
	}

	void start()
	{
		//
		// Enqueue all of the handlers right off the bat
		for (auto handler = 0; handler < m_count; handler++)
		{
			m_strand.post([&]() { next(); });
		}
	}

private:
	boost::asio::strand m_strand;
	std::string m_name;
	uint16_t m_count;

	void next()
	{
		std::cout << "Thread id: ";
		std::cout << std::this_thread::get_id();
		std::cout << " Name: " << m_name;
		std::cout << " - Count: " << m_count << std::endl;
		m_count--;
	}
};

int main()
{
	boost::asio::io_service ioService;
	boost::asio::io_service::work work(ioService);

	std::vector<std::thread> threads;
	for (auto thread : IRange<uint8_t>(1, std::thread::hardware_concurrency()))
	{
		threads.push_back(std::thread(
			[&ioService]()
			{
				ioService.run();
			}));
	}

	Countdown obj1(ioService, "One", 10);
	Countdown obj2(ioService, "Two", 12);

	obj1.start();
	obj2.start();

	for (auto& thread : threads)
	{
		thread.join();
	}

	return 0;
}
