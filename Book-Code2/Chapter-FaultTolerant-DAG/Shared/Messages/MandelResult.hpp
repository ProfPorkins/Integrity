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
