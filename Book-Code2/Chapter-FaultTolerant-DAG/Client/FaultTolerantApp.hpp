#ifndef _FAULTTOLERANTAPP_HPP_
#define _FAULTTOLERANTAPP_HPP_

#include "Mandelbrot.hpp"
#include "Shared/FaultTolerantFramework.hpp"
#include "Shared/Messages/DAGExampleResult.hpp"
#include "Shared/Messages/MandelFinishedResult.hpp"
#include "Shared/Messages/MandelResult.hpp"
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

	void moveLeft()		{ m_mandelbrot->moveLeft(); }
	void moveRight()	{ m_mandelbrot->moveRight(); }
	void moveUp()		{ m_mandelbrot->moveUp(); }
	void moveDown()		{ m_mandelbrot->moveDown(); }
	void zoomIn()		{ m_mandelbrot->zoomIn(); }
	void zoomOut()		{ m_mandelbrot->zoomOut(); }

private:
	FaultTolerantFramework m_ftFramework;

	std::shared_ptr<Mandelbrot> m_mandelbrot;
	std::atomic<bool> m_reportPrime;
	std::atomic<uint32_t> m_lastPrime;

	void processMandelResult(std::shared_ptr<Messages::MandelResult> taskResult);
	void processMandelFinishedResult(std::shared_ptr<Messages::MandelFinishedResult> taskResult);
	void processNextPrimeResult(std::shared_ptr<Messages::NextPrimeResult> taskResult);
	void processDAGExampleResult(std::shared_ptr<Messages::DAGExampleResult> taskResult);
};

#endif // _FAULTTOLERANTAPP_HPP_
