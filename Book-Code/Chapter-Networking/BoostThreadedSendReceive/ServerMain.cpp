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
#include <memory>
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

namespace ip = boost::asio::ip;

void handleNextMessage(std::shared_ptr<ip::tcp::socket> socket, uint16_t socketID)
{
	using boost::asio::buffer;
	using boost::system::error_code;
	//
	// Validate the socket is still open
	if (!socket->is_open()) return;
	
	//
	// Perform an asynchronous read so that we don't tie up
	// a thread waiting on just this socket.
	std::shared_ptr<uint16_t> bufferSize = std::make_shared<uint16_t>();
	boost::asio::async_read(*socket,buffer(bufferSize.get(), sizeof(uint16_t)),
		[socket, socketID, bufferSize](const error_code& error, std::size_t bytes)
		{
			if (!error)
			{
				std::string receiveString;
				receiveString.resize(*bufferSize);
				boost::asio::read(*socket, buffer(const_cast<char*>(receiveString.data()), *bufferSize));
				std::cout << "Received from " << socketID << " : " << receiveString;

				handleNextMessage(socket, socketID);
			}
		});
}

void handleNewConnection(boost::asio::io_service& ioService, std::shared_ptr<ip::tcp::acceptor> acceptor)
{
	static uint16_t socketID = 1;

	std::shared_ptr<ip::tcp::socket> socket = std::make_shared<ip::tcp::socket>(ioService);
	acceptor->async_accept(*socket, 
		[&ioService, acceptor, socket](const boost::system::error_code& error)
		{
			if (!error)
			{
				std::cout << "Received connection from: ";
				std::cout << socket->remote_endpoint();
				std::cout << " on thread id: ";
				std::cout << std::this_thread::get_id() << std::endl;

				//
				// With the connection made, start listening for messages over the socket
				handleNextMessage(socket, socketID);
				socketID++;
			}
			handleNewConnection(ioService, acceptor);
		});
}

int main()
{
	boost::asio::io_service ioService;
	boost::asio::io_service::work work(ioService);

	std::thread myThread(
		[&ioService]()
		{
			std::cout << "io_service thread id: ";
			std::cout << std::this_thread::get_id() << std::endl;
			ioService.run();
		});

	//
	// Only allowed to have one acceptor listening on a port at at time.
	// Therefore, creating it one time here and passing it into the handleNewConnection function.
	std::shared_ptr<ip::tcp::acceptor> acceptor = std::make_shared<ip::tcp::acceptor>(ioService, ip::tcp::endpoint(ip::tcp::v4(), 12345));

	handleNewConnection(ioService, acceptor);
	myThread.join();

	return 0;
}
