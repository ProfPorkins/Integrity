#include "ComputeServer.hpp"
#include "Shared/Threading/ThreadPool.hpp"

#include <iostream>
#include <thread>

#include <boost/asio.hpp>

bool parseClient(int argc, char* argv[], std::string& ip, std::string& port);

int main(int argc, char* argv[])
{
	auto ipClient = std::string{};
	auto portClient = std::string{};
	if (parseClient(argc, argv, ipClient, portClient))
	{
	boost::asio::io_service ioService;
	boost::asio::io_service::work work(ioService);

		std::thread thread = std::thread(
			[&ioService]()
			{
				ioService.run();
			});

		auto server = ComputeServer{};
		server.initialize(&ioService, ipClient, portClient);

		thread.join();
		std::cout << "Server finished" << std::endl;
	}
	else
	{
		std::cout << "Incorrect command line parameters - Server <client ip> <portnum>" << std::endl;
	}

	return 0;
}

// -----------------------------------------------------------------
//
// @details Extracts the client ip and port from the command line parameters.
//
// -----------------------------------------------------------------
bool parseClient(int argc, char* argv[], std::string& ip, std::string& port)
{
	auto success = bool{ false };
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
