#include <array>
#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include "message.pb.h"

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
	sample::Line line;
	auto bufferSize = std::make_shared<decltype(line.ByteSize())>();
	boost::asio::async_read(*socket, boost::asio::buffer(bufferSize.get(), sizeof(decltype(line.ByteSize()))),
		[socket, socketID, bufferSize](const error_code& error, std::size_t bytes)
		{
			if (!error)
			{
				boost::asio::streambuf buffer;

				boost::asio::streambuf::mutable_buffers_type bufs = buffer.prepare(*bufferSize);
				boost::asio::read(*socket, bufs);
				buffer.commit(*bufferSize);

				std::istream is(&buffer);
				sample::Line line;
				line.ParseFromIstream(&is);
				std::cout << "Received from " << socketID << " : " << line.message() << line.line() << std::endl;

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
