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

#ifndef _FAULTTOLERANTAPP_HPP_
#define _FAULTTOLERANTAPP_HPP_

//
// Disable some compiler warnings that come from boost
// Have to include this first to prevent Windows.h from crying over spilled milk
#pragma warning(push)
#pragma warning(disable : 4267)
#pragma warning(disable : 4996)
#include <boost/asio.hpp>
#pragma warning(pop)

#include <windows.h>

#include "Mandelbrot.hpp"
#include "ServerSet.hpp"

#include <atomic>
#include <cstdint>
#include <memory>
#include <thread>
#include <unordered_map>
#include <vector>

namespace ip = boost::asio::ip;

// ------------------------------------------------------------------
//
// @details This class provides the basic application framework to demonstrate
// a scalable and distributed architecture.  The application allows user 
// interaction with a Mandelbrot set.
//
// ------------------------------------------------------------------
class FaultTolerantApp
{
public:
	FaultTolerantApp(HWND hwnd, uint16_t sizeX, uint16_t sizeY);

	bool initialize();
	void terminate();

	void pulse();

	void moveLeft()		{ m_mandelbrot->moveLeft(); }
	void moveRight()	{ m_mandelbrot->moveRight(); }
	void moveUp()		{ m_mandelbrot->moveUp(); }
	void moveDown()		{ m_mandelbrot->moveDown(); }
	void zoomIn()		{ m_mandelbrot->zoomIn(); }
	void zoomOut()		{ m_mandelbrot->zoomOut(); }

private:
	std::shared_ptr<Mandelbrot> m_mandelbrot;
	std::atomic<bool> m_reportPrime;
	std::atomic<uint32_t> m_lastPrime;

	boost::asio::io_service m_ioService;
	std::vector<std::unique_ptr<std::thread>> m_threadsIO;
	ip::tcp::acceptor m_acceptor;
	std::unique_ptr<boost::asio::io_service::work> m_threadWork;
	ServerSet m_servers;

	bool m_running;
	std::unordered_map<Messages::Type, std::function<void (ServerID_t)>> m_messageCommand;

	void prepareCommandMap();
	void handleNewConnection();
	void handleNextMessage(ServerID_t serverId);
	void processTaskRequest(ServerID_t serverId);
	void processMandelResult(ServerID_t serverId);
	void processNextPrimeResult(ServerID_t serverId);
};

#endif // _FAULTTOLERANTAPP_HPP_
