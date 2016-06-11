#ifndef _DAGEXAMPLETASK_HPP_
#define _DAGEXAMPLETASK_HPP_

#include "Shared/Messages/DAGExample.hpp"
#include "Task.hpp"

#include <string>

namespace Tasks
{
	// -----------------------------------------------------------------
	//
	// @details This class represents a "DAG Example" task.  It doesn't do
	// anything other than sleep for a bit to help illustrate the dependencies
	// at work.
	//
	// -----------------------------------------------------------------
	class DAGExampleTask : public Task
	{
	public:
		DAGExampleTask(std::string name) :
			m_name(name)
		{
		}

		DAGExampleTask(std::shared_ptr<ip::tcp::socket> socket, Messages::DAGExample& message) :
			Task(socket, message.getTaskId())
		{
			m_name = message.m_message.name();
		}

		virtual void execute() override;

	protected:
		virtual std::shared_ptr<Messages::Message> getMessage() override;
		virtual std::shared_ptr<Messages::Message> completeCustom(boost::asio::io_service& ioService) override;

	private:
		std::string m_name;
	};
}

#endif // _DAGEXAMPLETASK_HPP_
