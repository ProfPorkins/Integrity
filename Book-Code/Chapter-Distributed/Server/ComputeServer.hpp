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

namespace std
{
	// ------------------------------------------------------------------
	//
	// @details This provides a std::hash for the Messages::Type enum so
	// that it can be used in a std::unordered_map.
	// Found this code on Stack Overflow at: http://stackoverflow.com/questions/18837857/cant-use-enum-class-as-unordered-map-key
	//
	// ------------------------------------------------------------------
	template<>
	struct hash<Messages::Type>
	{
		typedef Messages::Type argument_type;
		typedef std::underlying_type<argument_type>::type underlying_type;
		typedef std::hash<underlying_type>::result_type result_type;
		result_type operator()(const argument_type& arg) const
		{
			std::hash<underlying_type> hasher;
			return hasher(static_cast<underlying_type>(arg));
		}
	};
}

// ------------------------------------------------------------------
//
// @details This class represents the primary application component
// for the compute server.  Its purpose is to accept client connections
// and work messages.  As work messages are received, they are decoded
// and added to the local work queue so that a worker thread can
// begin its work.
//
// ------------------------------------------------------------------
class ComputeServer
{
public:
	ComputeServer();
	void initialize(boost::asio::io_service& ioService, const std::string& ipClient, const std::string& portClient);

private:
	std::array<uint8_t, 1> m_messageType;
	std::unordered_map<Messages::Type, std::function<void (std::shared_ptr<ip::tcp::socket>)>> m_messageCommand;

	void prepareCommandMap();
	void connectToClient(boost::asio::io_service& ioService, const std::string& ipClient, const std::string& portClient);
	void waitOnTask(std::shared_ptr<ip::tcp::socket> socket);

};

#endif // _COMPUTESERVER_HPP_
