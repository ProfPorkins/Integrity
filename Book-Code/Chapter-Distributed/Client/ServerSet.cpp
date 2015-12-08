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

#include "ServerSet.hpp"

// ------------------------------------------------------------------
//
// @details Adds a new server to the set.
//
// ------------------------------------------------------------------
void ServerSet::add(Server server)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	m_servers[server.id] = server;
}

// ------------------------------------------------------------------
//
// @details Returns a reference to the requested server, based upon id.
// A boost::optional return is used and set to true/false depending
// upon whether or not the server id was found.
//
// ------------------------------------------------------------------
boost::optional<Server&> ServerSet::get(ServerID_t id)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	boost::optional<Server&> server;
	auto it = m_servers.find(id);
	if (it != m_servers.end())
	{
		server = it->second;
	}

	return server;
}

// ------------------------------------------------------------------
//
// @details Returns true/false based upon the existence of the requested
// server...based upon id.
//
// ------------------------------------------------------------------
bool ServerSet::exists(ServerID_t id)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	return (m_servers.find(id) != m_servers.end());
}
