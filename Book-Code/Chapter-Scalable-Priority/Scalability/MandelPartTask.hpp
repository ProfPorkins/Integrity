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

#ifndef _MANDELPARTTASK_HPP_
#define _MANDELPARTTASK_HPP_

#include "Task.hpp"

// ------------------------------------------------------------------
//
// @details This task computes a single row in a mandelbrot image
//
// ------------------------------------------------------------------
class MandelPartTask : public Task
{
public:
	MandelPartTask(uint16_t* imageRow, uint16_t resolution, uint16_t maxIterations, double y, double startX, double endX, std::function<void ()> onComplete, Priority priority);
	virtual void execute() override;

private:
	uint16_t* m_imageRow;
	uint16_t m_resolution;
	uint16_t m_maxIterations;
	double m_y;
	double m_startX;
	double m_endX;
};

#endif // _MANDELPARTTASK_HPP_
