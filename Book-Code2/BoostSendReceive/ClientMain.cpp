#include "IRange.hpp"
#include "message.pb.h"

#include <array>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <memory>
#include <ostream>
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

void sendMessages(std::shared_ptr<ip::tcp::socket> socket)
{
	for (auto line : IRange<uint16_t>(1, 10))
	{
		learn::Line msg;

		msg.set_message("This is line: ");
		msg.set_line(line);

		boost::asio::streambuf buffer;
		std::ostream os(&buffer);
		msg.SerializeToOstream(&os);

		auto sendSize = msg.ByteSize();
		boost::asio::write(*socket, boost::asio::buffer(&sendSize, sizeof(decltype(sendSize))));
		auto size = boost::asio::write(*socket, buffer);
		buffer.consume(size);

		std::cout << "Sent: " << msg.message() << msg.line() << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	socket->close();
	socket->get_io_service().stop();
}

int main()
{
	GOOGLE_PROTOBUF_VERIFY_VERSION;

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

	std::shared_ptr<ip::tcp::socket> socket = std::make_shared<ip::tcp::socket>(ioService);
	boost::asio::async_connect(*socket, iterator,
		[socket](const boost::system::error_code& error, ip::tcp::resolver::iterator iterator)
		{
			if (!error)
			{
				ip::tcp::endpoint endpoint(*iterator);
				std::cout << "Made a connection to: " << endpoint;
				std::cout << " on thread id: ";
				std::cout << std::this_thread::get_id();
				std::cout << std::endl;

				//
				// With the connection made, send some data
				sendMessages(socket);
			}
		});

	myThread.join();

	return 0;
}
