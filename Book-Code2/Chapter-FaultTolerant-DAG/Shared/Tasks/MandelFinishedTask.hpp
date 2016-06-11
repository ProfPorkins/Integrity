#ifndef _MANDELFINISHEDTASK_HPP_
#define _MANDELFINISHEDTASK_HPP_

#include "Shared/Messages/MandelFinished.hpp"
#include "Task.hpp"

#include <memory>

namespace Tasks
{
	// -----------------------------------------------------------------
	//
	// @details This class represents the indicator that the Mandelbrot
	// image has completed computation.
	//
	// -----------------------------------------------------------------
	class MandelFinishedTask : public Task
	{
	public:
		MandelFinishedTask() 
		{
		}

		MandelFinishedTask(std::shared_ptr<ip::tcp::socket> socket, Messages::MandelFinished& message) :
			Task(socket, message.getTaskId())
		{
		}

		virtual void execute() override;

	protected:
		virtual std::shared_ptr<Messages::Message> getMessage() override;
		virtual std::shared_ptr<Messages::Message> completeCustom(boost::asio::io_service& ioService) override;

	};
}

#endif // _MANDELFINISHEDTASK_HPP_
