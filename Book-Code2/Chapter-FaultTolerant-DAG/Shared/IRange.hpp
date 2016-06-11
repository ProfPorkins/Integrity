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
