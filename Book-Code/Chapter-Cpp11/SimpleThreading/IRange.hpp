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

#ifndef _IRANGE_HPP_
#define _IRANGE_HPP_

template <typename T>
class IRange
{
public:
	class iterator
	{
	public:
		T operator *() const { return m_position; }
		const iterator &operator ++()
		{
			m_position += m_increment;
			return *this; 
		}

		bool operator ==(const iterator &rhs) const
		{ 
			return m_position == rhs.m_position;
		}
		bool operator !=(const iterator &rhs) const
		{ 
			return m_position < rhs.m_position;
		}

	private:
		iterator(T start) : 
			m_position(start),
			m_increment(1)
		{ }

		iterator(T start, T increment) : 
			m_position(start),
			m_increment(increment)
		{ }

		T m_position;
		T m_increment;

		friend class IRange;
	};

	IRange(T begin, T end) : 
		m_begin(begin), 
		m_end(end + 1)
	{ }

	IRange(T begin, T end, T increment) :
		m_begin(begin, increment),
		m_end(end + 1, increment)
	{ }

	iterator begin() const { return m_begin; }
	iterator end() const { return m_end; }

private:
	iterator m_begin;
	iterator m_end;
};

#endif // _IRANGE_HPP_
