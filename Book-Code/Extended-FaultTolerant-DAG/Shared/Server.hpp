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

#ifndef _SERVER_HPP_
#define _SERVER_HPP_

#include <array>
#include <memory>
#include <string>

//
// Disable some compiler warnings that come from boost
#pragma warning(push)
#pragma warning(disable : 4267)
#pragma warning(disable : 4996)
#include <boost/asio.hpp>
#pragma warning(pop)

namespace ip = boost::asio::ip;
typedef uint16_t ServerID_t;

// -----------------------------------------------------------------
//
// @details This structure is used to hold the information about a
// distributed computing server.  It creates a unique id for each
// server that is used as a lookup id (think hash table) for referencing
// the details of the server.
//
// -----------------------------------------------------------------
struct Server
{
	Server() 
	{
	}

	Server(std::shared_ptr<ip::tcp::socket> socket) :
		socket(socket)
	{
		static auto newId = ServerID_t{ 0 };
		this->id = newId++;

		this->strand = std::make_shared<boost::asio::strand>(socket->get_io_service());
	}

	ServerID_t id;
	std::shared_ptr<ip::tcp::socket> socket;
	std::shared_ptr<boost::asio::strand> strand;
	std::array<uint8_t, 1> messageType;			// Used to accept the incoming message type
};

#endif // _SERVER_HPP_
