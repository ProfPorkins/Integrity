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

#ifndef _MANDELRESULTMESSAGE_HPP_
#define _MANDELRESULTMESSAGE_HPP_

#include "MandelResult.pb.h"
#include "ResultMessage.hpp"

#include <memory>
#include <vector>

namespace Messages
{
	// -----------------------------------------------------------------
	//
	// @details This is used to send the result of the mandelbrot part
	// computation from the compute node back to the client.
	//
	// -----------------------------------------------------------------
	class MandelResult : public ResultMessage<PBMessages::MandelResult>
	{
	public:
		MandelResult() :
			ResultMessage(Messages::Type::MandelResult)
		{
		}

		MandelResult(uint64_t taskId, uint16_t startRow, uint16_t endRow, std::vector<uint16_t> pixels) :
			ResultMessage(Messages::Type::MandelResult, taskId)
		{
			m_message.set_startrow(startRow);
			m_message.set_endrow(endRow);
			for (auto pixel : pixels)
			{
				m_message.add_pixel(pixel);
			}
		}

		uint16_t getStartRow()											{ return m_message.startrow(); }
		uint16_t getEndRow()											{ return m_message.endrow(); }
		const google::protobuf::RepeatedField<uint32_t>& getPixels()	{ return m_message.pixel(); }
	};
}

#endif // _MANDELRESULTMESSAGE_HPP_
