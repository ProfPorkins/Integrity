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

#ifndef _NEXTPRIMETASK_HPP_
#define _NEXTPRIMETASK_HPP_

#include "Task.hpp"

#include <cstdint>

// ------------------------------------------------------------------
//
// @details This task computes a the next prime number in the series
//
// ------------------------------------------------------------------
class NextPrimeTask : public Task
{
public:
	NextPrimeTask(uint32_t lastPrime, uint32_t& nextPrime, std::function<void ()> onComplete, Priority priority);

	virtual void execute() override;

private:
	uint32_t m_lastPrime;
	uint32_t& m_nextPrime;

	bool isPrime(uint32_t value);
};

#endif // _NEXTPRIMETASK_HPP_
