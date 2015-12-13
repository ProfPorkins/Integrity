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

#ifndef _SERVERSET_HPP_
#define _SERVERSET_HPP_

#include "Server.hpp"

#include <memory>
#include <mutex>
#include <unordered_map>
#include <unordered_set>

//
// Disable some compiler warnings that come from boost
#pragma warning(push)
#pragma warning(disable : 4267)
#pragma warning(disable : 4996)
#include <boost/asio.hpp>
#include <boost/optional.hpp>
#pragma warning(pop)

namespace ip = boost::asio::ip;

// -----------------------------------------------------------------
//
// @details The purpose of this class is to provide a synchronized
// wrapper around the use and access of the list of currently 
// available servers.
//
// -----------------------------------------------------------------
class ServerSet
{
public:
	void add(Server server);
	boost::optional<Server&> get(ServerID_t id);
	bool exists(ServerID_t id);
	std::unordered_map<ServerID_t, Server> getServers() 
	{ 
		std::lock_guard<std::mutex> lock(m_mutex);
		return m_servers; 
	}
	std::unordered_set<ServerID_t> removeDisconnected();


private:
	std::unordered_map<ServerID_t, Server> m_servers;
	std::mutex m_mutex;
};

#endif // _SERVERSET_HPP_
