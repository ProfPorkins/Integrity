#ifndef _TASK_HPP_
#define _TASK_HPP_

#include "Shared/Messages/Message.hpp"

#include <chrono>
#include <functional>
#include <memory>

#include <boost/asio.hpp>

namespace ip = boost::asio::ip;

namespace Tasks
{
	// -----------------------------------------------------------------
	//
	// @details This is the base class from which all tasks are derived.
	//
	// -----------------------------------------------------------------
	class Task
	{
	public:
		Task();
		Task(std::shared_ptr<ip::tcp::socket> socket, uint64_t id);

		virtual ~Task() {}	// Virtual destructor to allow derived class destructors to correctly get called

		void send(std::shared_ptr<ip::tcp::socket> socket, boost::asio::strand& strand);
		virtual void execute() = 0;
		void complete(boost::asio::io_service& ioService);

		uint64_t getId()							{ return m_id; }

	protected:
		uint64_t m_id;
		std::shared_ptr<ip::tcp::socket> m_socket;

		virtual std::shared_ptr<Messages::Message> getMessage() = 0;
		virtual std::shared_ptr<Messages::Message> completeCustom(boost::asio::io_service& ioService) = 0;
	};
}

#endif // _TASK_HPP_
