#ifndef _MANDELTASK_HPP_
#define _MANDELTASK_HPP_

#include "Shared/Messages/MandelMessage.hpp"
#include "Task.hpp"

#include <memory>
#include <vector>

namespace Tasks
{
	// -----------------------------------------------------------------
	//
	// @details This class represents a sub-image Mandelbrot work unit.
	// This class computes and returns the results for a specified
	// region of the Mandelbrot image.
	//
	// -----------------------------------------------------------------
	class MandelTask : public Task
	{
	public:
		MandelTask(uint16_t startRow, uint16_t endRow, uint16_t sizeX, double startX, double startY, double deltaX, double deltaY, uint16_t maxIterations) :
			m_startRow(startRow),
			m_endRow(endRow),
			m_sizeX(sizeX),
			m_startX(startX),
			m_startY(startY),
			m_deltaX(deltaX),
			m_deltaY(deltaY),
			m_maxIterations(maxIterations)
		{
		}

		MandelTask(std::shared_ptr<ip::tcp::socket> socket, Messages::MandelMessage& message) :
			Task(socket, message.getTaskId()),
			m_startRow(message.m_message.startrow()),
			m_endRow(message.m_message.endrow()),
			m_sizeX(message.m_message.sizex()),
			m_startX(message.m_message.startx()),
			m_startY(message.m_message.starty()),
			m_deltaX(message.m_message.deltax()),
			m_deltaY(message.m_message.deltay()),
			m_maxIterations(message.m_message.maxiterations())
		{
		}

		virtual void execute() override;

	protected:
		virtual std::shared_ptr<Messages::Message> getMessage() override;
		virtual std::shared_ptr<Messages::Message> completeCustom(boost::asio::io_service& ioService) override;

	private:
		uint16_t m_startRow;
		uint16_t m_endRow;
		uint16_t m_sizeX;
		double m_startX;
		double m_startY;
		double m_deltaX;
		double m_deltaY;
		uint16_t m_maxIterations;
		std::vector<uint16_t> m_pixels;
	};
}

#endif // _MANDELTASK_HPP_
