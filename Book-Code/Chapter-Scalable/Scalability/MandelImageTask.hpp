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

#ifndef _MANDELIMAGETASK_HPP_
#define _MANDELIMAGETASK_HPP_

#include "Task.hpp"

#include <array>
#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <string>

// ------------------------------------------------------------------
//
// @details This work item manages the computation of a full mandelbrot image.
// It breaks down the computation into further work items that each
// compute a single row of the image.
//
// ------------------------------------------------------------------
class MandelImageTask : public Task
{
public:
	MandelImageTask(double startX, double endX, double startY, double endY, uint16_t maxIterations, uint16_t sizeX, uint16_t sizeY, uint8_t* pixels, uint16_t stride, std::function<void ()> onComplete);

	virtual void execute() override;

private:
	struct Color
	{
		uint8_t r;
		uint8_t g;
		uint8_t b;
	};
	std::array<Color, 768> m_colors;
	std::atomic<uint16_t> m_partsFinished;
	std::condition_variable m_imageFinished;
	std::mutex m_mutexImageFinished;

	double m_startX;
	double m_endX;
	double m_startY;
	double m_endY;
	uint16_t m_maxIterations;
	uint16_t m_sizeX;
	uint16_t m_sizeY;
	uint8_t* m_pixels;
	uint16_t m_stride;

	std::unique_ptr<uint16_t[]> m_image;

	void prepareColors();
	void copyToPixels();
	void completePart();
};

#endif // _MANDELIMAGETASK_HPP_
