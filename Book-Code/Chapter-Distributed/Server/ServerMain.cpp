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

#include "ComputeServer.hpp"
#include "Shared/Threading/ThreadPool.hpp"

#include <iostream>
#include <thread>

#include <boost/asio.hpp>

bool parseClient(int argc, char* argv[], std::string& ip, std::string& port);

int main(int argc, char* argv[])
{
	std::string ipClient;
	std::string portClient;
	if (parseClient(argc, argv, ipClient, portClient))
	{
		boost::asio::io_service ioService;
		boost::asio::io_service::work work(ioService);

		std::thread thread = std::thread(
			[&ioService]() 
			{ 
				ioService.run();
			});

		ComputeServer server;
		server.initialize(ioService, ipClient, portClient);

		thread.join();
		//
		// Need to gracefully shut down the thread pool.
		ThreadPool::instance()->terminate();
	}
	else
	{
		std::cout << "Incorrect command line parameters - Server <client ip> <portnum>" << std::endl;
	}

	return 0;
}

// ------------------------------------------------------------------
//
// @details Extracts the client ip and port from the command line parameters.
//
// ------------------------------------------------------------------
bool parseClient(int argc, char* argv[], std::string& ip, std::string& port)
{
	bool success = false;
	if (argc == 3)
	{
		try
		{
			ip = std::string(argv[1]);
			port = std::string(argv[2]);
			success = true;
		}
		catch (std::exception& ex)
		{
			std::cout << "Unable to parse the client ip and port: " << ex.what() << std::endl;
		}
	}

	return success;
}
