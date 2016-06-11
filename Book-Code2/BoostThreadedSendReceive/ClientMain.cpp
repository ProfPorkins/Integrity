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

class Sender
{
public:
	Sender() :
		m_socket(nullptr),
		m_strand(nullptr)
	{
	}

	void startSending(std::shared_ptr<ip::tcp::socket> socket, std::shared_ptr<boost::asio::strand> strand)
	{
		m_socket = socket;
		m_strand = strand;

		m_thread = std::make_shared<std::thread>(
			[this]()
			{
				sendMessages();
			});
	}

	void join() { m_thread->join(); }

private:
	std::shared_ptr<std::thread> m_thread;
	std::shared_ptr<ip::tcp::socket> m_socket;
	std::shared_ptr<boost::asio::strand> m_strand;

	void sendMessages()
	{
		for (auto line : IRange<uint16_t>(1, 5))
		{
			m_strand->post([this, line]() { sendMessage(line); });
		}
	}

	void sendMessage(uint16_t line)
	{
		sample::Line message;

		message.set_message("This is line: ");
		message.set_line(line);

		boost::asio::streambuf buffer;
		std::ostream os(&buffer);
		message.SerializeToOstream(&os);

		auto sendSize = message.ByteSize();
		boost::asio::write(*m_socket, boost::asio::buffer(&sendSize, sizeof(decltype(sendSize))));
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
		auto size = boost::asio::write(*m_socket, buffer);
		buffer.consume(size);

		std::cout << "Sent: " << message.message() << message.line() << std::endl;
	}
};

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

	std::thread myThread2(
		[&ioService]()
		{
			std::cout << "io_service thread id: " << std::this_thread::get_id() << std::endl;
			ioService.run();
		});

	//
	// Create some threads to perform multiple writes
	std::vector<std::shared_ptr<Sender>> senders;
	for (auto sender : IRange<uint8_t>(1, 2))
	{
		senders.push_back(std::make_shared<Sender>());
	}

	ip::tcp::resolver::query query("127.0.0.1", "12345");
	ip::tcp::resolver resolver(ioService);
	ip::tcp::resolver::iterator iterator = resolver.resolve(query);

	std::shared_ptr<ip::tcp::socket> socket = std::make_shared<ip::tcp::socket>(ioService);
	boost::asio::async_connect(*socket, iterator,
		[socket, &ioService, &senders](const boost::system::error_code& error, ip::tcp::resolver::iterator iterator)
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
				auto strand = std::make_shared<boost::asio::strand>(ioService);
				for (auto sender : senders)
				{
					sender->startSending(socket, strand);
				}
			}
		});

	myThread.join();

	return 0;
}
