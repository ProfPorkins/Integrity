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

#include <array>
#include <cstdint>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>

//
// Disable some compiler warnings that come from boost
#pragma warning(push)
#pragma warning(disable : 4267)
#pragma warning(disable : 4996)
#include <boost/asio.hpp>
#pragma warning(pop)

namespace ip = boost::asio::ip;

int main()
{
	boost::asio::io_service ioService;
	boost::asio::io_service::work work(ioService);

	std::thread myThread(
		[&ioService]()
		{
			std::cout << "io_service thread id: " << std::this_thread::get_id() << std::endl;
			ioService.run();
		});

	ip::tcp::resolver::query query("127.0.0.1", "12345");
	ip::tcp::resolver resolver(ioService);
	ip::tcp::resolver::iterator iterator = resolver.resolve(query);

	ip::tcp::socket socket(ioService);
	boost::asio::async_connect(socket, iterator,
		[](const boost::system::error_code& error, 
			ip::tcp::resolver::iterator iterator)
		{
			if (!error)
			{
				ip::tcp::endpoint endpoint(*iterator);
				std::cout << "Made a connection to: " << endpoint;
				std::cout << " on thread id: ";
				std::cout << std::this_thread::get_id() << std::endl;
			}
		});

	myThread.join();

	return 0;
}
