#include "ServerSet.hpp"

// -----------------------------------------------------------------
//
// @details Adds a new server to the set.
//
// -----------------------------------------------------------------
void ServerSet::add(Server server)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	m_servers[server.id] = server;
}

// -----------------------------------------------------------------
//
// @details Returns a reference to the requested server, based upon id.
// A boost::optional return is used and set to true/false depending
// upon whether or not the server id was found.
//
// -----------------------------------------------------------------
boost::optional<Server&> ServerSet::get(ServerID_t id)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	auto server = boost::optional<Server&>();
	auto it = m_servers.find(id);
	if (it != m_servers.end())
	{
		server = it->second;
	}

	return server;
}

// -----------------------------------------------------------------
//
// @details Returns true/false based upon the existence of the requested
// server...based upon id.
//
// -----------------------------------------------------------------
bool ServerSet::exists(ServerID_t id)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	return (m_servers.find(id) != m_servers.end());
}

// -----------------------------------------------------------------
//
// @details This method goes through and removes any servers whose
// sockets are no longer open.  The return value is the set of
// servers removed.
//
// -----------------------------------------------------------------
std::unordered_set<ServerID_t> ServerSet::removeDisconnected()
{
	std::lock_guard<std::mutex> lock(m_mutex);

	//
	// Find servers with disconnected sockets first
	// Note: Can not use std::remove_if with associative containers.  Therefore,
	// doing this in two steps.
	auto removeMe = std::unordered_set<ServerID_t>();
	for (auto server : m_servers)
	{
		if (!server.second.socket->is_open())
		{
			removeMe.insert(server.first);
		}
	}

	//
	// Then remove them from our list
	for (auto server : removeMe)
	{
		m_servers.erase(server);
	}

	return std::move(removeMe);
}
