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

#include "MandelResultMessage.hpp"

#include "Shared/IRange.hpp"

namespace Messages
{
	// -----------------------------------------------------------------
	//
	// @details Encodes the mandelbrot part results.
	//
	// -----------------------------------------------------------------
	void MandelResultMessage::encodeResult()
	{
		this->encode(m_startRow);
		this->encode(m_endRow);

		std::uint32_t pixelCount = static_cast<std::uint32_t>(m_pixels.size());
		this->encode(pixelCount);
		for (auto pixel : m_pixels)
		{
			this->encode(pixel);
		}
	}

	// -----------------------------------------------------------------
	//
	// @details Decodes the mandelbrot part results.
	//
	// -----------------------------------------------------------------
	void MandelResultMessage::decodeResult()
	{
		this->decode(m_startRow);
		this->decode(m_endRow);

		std::uint32_t pixelCount = 0;
		this->decode(pixelCount);
		m_pixels.resize(pixelCount);

		uint16_t* pixels = m_pixels.data();
		for (auto pixel : IRange<uint32_t>(0, pixelCount - 1))
		{
			this->decode(pixels[pixel]);
		}
	}
}
