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
