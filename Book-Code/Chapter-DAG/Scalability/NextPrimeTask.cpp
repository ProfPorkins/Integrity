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

#include "NextPrimeTask.hpp"

#include "IRange.hpp"

NextPrimeTask::NextPrimeTask(uint32_t lastPrime, uint32_t& nextPrime, std::function<void ()> onComplete) :
	Task(onComplete),
	m_lastPrime(lastPrime),
	m_nextPrime(nextPrime)
{
}

// ------------------------------------------------------------------
//
// @details The worst known algorithm for finding the next prime number :)
//
// ------------------------------------------------------------------
void NextPrimeTask::execute()
{
	m_nextPrime += 2;
	while (!isPrime(m_nextPrime))
	{
		m_nextPrime += 2;
	}
}

// ------------------------------------------------------------------
//
// @details Test a value for the prime property.  Could use a little
// work to eliminate all the returns, making a single point of return.
//
// ------------------------------------------------------------------
bool NextPrimeTask::isPrime(uint32_t value)
{
	if (value == 2) return true;

	if (value % 2 == 0 || value <= 1) return false;

	for (auto mod : IRange<decltype(value)>(3, static_cast<decltype(value)>(std::sqrt(value)), 2))
	{
		if (value % mod == 0)
		{
			return false;
		}
	}

	return true;
}
