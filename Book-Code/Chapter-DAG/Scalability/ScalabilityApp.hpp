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

#ifndef _SCALABILITYAPP_HPP_
#define _SCALABILITYAPP_HPP_

#include "Mandelbrot.hpp"

#include <atomic>
#include <cstdint>
#include <memory>
#include <mutex>

// ------------------------------------------------------------------
//
// @details This class provides the basic application framework to demonstrate
// a scalable architecture.  The application allows user interaction 
// with a Mandelbrot set, along with a low-priority task of generating 
// prime numbers.
//
// ------------------------------------------------------------------
class ScalabilityApp
{
public:
	ScalabilityApp(HWND hwnd, uint16_t sizeX, uint16_t sizeY);

	void pulse();

	void moveLeft()		{ m_mandelbrot->moveLeft(); }
	void moveRight()	{ m_mandelbrot->moveRight(); }
	void moveUp()		{ m_mandelbrot->moveUp(); }
	void moveDown()		{ m_mandelbrot->moveDown(); }
	void zoomIn()		{ m_mandelbrot->zoomIn(); }
	void zoomOut()		{ m_mandelbrot->zoomOut(); }

private:
	std::shared_ptr<Mandelbrot> m_mandelbrot;
	uint32_t m_currentPrime;
	std::atomic<bool> m_reportPrime;
	uint32_t m_lastPrime;

	void startNextPrime();
	void startChapterDemo();
};

#endif // _SCALABILITYAPP_HPP_
