#ifndef _FAULTTOLERANTAPP_HPP_
#define _FAULTTOLERANTAPP_HPP_

#include "Shared/FaultTolerantFramework.hpp"
#include "Shared/Messages/NextPrimeResult.hpp"

//
// This must be included after FaultTolerantFramework.hpp because of boost/asio.hpp and the Winsock.h include problem.
#include <windows.h>

#include <atomic>
#include <cstdint>
#include <memory>
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

private:
	FaultTolerantFramework m_ftFramework;

	std::atomic<bool> m_reportPrime;
	std::atomic<uint32_t> m_lastPrime;

	void processNextPrimeResult(std::shared_ptr<Messages::NextPrimeResult> taskResult);
};

#endif // _FAULTTOLERANTAPP_HPP_
