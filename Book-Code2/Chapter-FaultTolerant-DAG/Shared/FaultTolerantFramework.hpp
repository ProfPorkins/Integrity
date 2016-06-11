#ifndef _FAULTTOLERANTFRAMEWORK_HPP_
#define _FAULTTOLERANTFRAMEWORK_HPP_

//
// Disable some compiler warnings that come from boost
// Have to include this first to prevent Windows.h from crying over spilled milk
#pragma warning(push)
#pragma warning(disable : 4267)
#pragma warning(disable : 4996)
#include <boost/asio.hpp>
#pragma warning(pop)

#include "ServerSet.hpp"
#include "Messages/Message.hpp"
#include "TaskRequestQueue.hpp"

#include <atomic>
#include <functional>
#include <iostream>
#include <memory>
#include <thread>
#include <unordered_map>
#include <vector>

namespace ip = boost::asio::ip;

// ------------------------------------------------------------------
//
// @details This class provides the basic fault-tolerant framework upon
// which applications are constructed.
//
// ------------------------------------------------------------------
class FaultTolerantFramework
{
public:
	FaultTolerantFramework();

	bool initialize();
	void terminate();

	template<typename Message>
	void registerHandler(Messages::Type type, std::function<void(std::shared_ptr<Message>)> handler)
	{
		m_messageCommand[type] = [this, handler](ServerID_t serverId)
		{ 
			//std::chrono::time_point<std::chrono::high_resolution_clock, std::chrono::nanoseconds> now = std::chrono::high_resolution_clock::now();
			//std::cout << "Received Message" << std::fixed << std::setprecision(10) << (now.time_since_epoch().count() / 1000000000.0) << std::endl;

			auto message = std::make_shared<Message>();
			Messages::read(*message, m_servers.get(serverId)->socket);

			//
			// Let the task queue know this task result has been recieved
			if (TaskRequestQueue::instance()->finalizeTask(message->getTaskId(), true, true))
			{
				handler(message);
			}
			else
			{
				std::cout << "Not finalized" << std::endl;
			}
		};
	}

private:
	boost::asio::io_service m_ioService;
	std::vector<std::unique_ptr<std::thread>> m_threadsIO;
	ip::tcp::acceptor m_acceptor;
	std::unique_ptr<boost::asio::io_service::work> m_threadWork;
	ServerSet m_servers;

	std::atomic<bool> m_running;
	std::unordered_map<Messages::Type, std::function<void (ServerID_t)>> m_messageCommand;

	void prepareInternalHandlers();
	void handleNewConnection();
	void handleNextMessage(ServerID_t serverId);
};

#endif // _FAULTTOLERANTFRAMEWORK_HPP_
