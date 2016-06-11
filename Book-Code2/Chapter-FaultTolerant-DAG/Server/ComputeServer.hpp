#ifndef _COMPUTESERVER_HPP_
#define _COMPUTESERVER_HPP_

#include <memory>
#include <unordered_map>
//
// Disable some compiler warnings that come from boost
#pragma warning(push)
#pragma warning(disable : 4267)
#pragma warning(disable : 4996)
#include <boost/asio.hpp>
#pragma warning(pop)

#include "Shared/Messages/Message.hpp"

namespace ip = boost::asio::ip;

// -----------------------------------------------------------------
//
// @details This class represents the primary application component
// for the compute server.  Its purpose is to accept client connections
// and work messages.  As work messages are received, they are added 
// to the local work queue so that a worker thread can begin its work.
//
// -----------------------------------------------------------------
class ComputeServer
{
public:
	void initialize(boost::asio::io_service* ioService, const std::string& ipClient, const std::string& portClient);

private:
	std::array<uint8_t, 1> m_messageType;
	std::unordered_map<Messages::Type, std::function<void (std::shared_ptr<ip::tcp::socket>)>> m_messageCommand;

	void prepareCommandMap();
	void connectToClient(boost::asio::io_service* ioService, const std::string& ipClient, const std::string& portClient);
	void handleNextTask(std::shared_ptr<ip::tcp::socket> socket);

};

#endif // _COMPUTESERVER_HPP_
