#ifndef _TASK_HPP_
#define _TASK_HPP_

#include "Shared/Messages/Message.hpp"

#include <functional>
#include <memory>

#include <boost/asio.hpp>

namespace ip = boost::asio::ip;

namespace Tasks
{
	enum class Priority : uint8_t
	{
		High = 1,
		Normal = 2,
		Low = 3
	};

	// -----------------------------------------------------------------
	//
	// @details This is the base class from which all tasks are derived.
	//
	// -----------------------------------------------------------------
	class Task
	{
	public:
		Task(Priority priority = Priority::Normal);
		Task(std::shared_ptr<ip::tcp::socket> socket, uint64_t id);

		virtual ~Task() {}	// Virtual destructor to allow derived class destructors to correctly get called

		void send(std::shared_ptr<ip::tcp::socket> socket, boost::asio::strand& strand);
		virtual void execute() = 0;
		void complete(boost::asio::io_service& ioService);

		uint64_t getId()							{ return m_id; }
		Priority getPriority() const				{ return m_priority; }

	protected:
		uint64_t m_id;
		Priority m_priority;
		std::shared_ptr<ip::tcp::socket> m_socket;

		virtual std::shared_ptr<Messages::Message> getMessage() = 0;
		virtual std::shared_ptr<Messages::Message> completeCustom(boost::asio::io_service& ioService) = 0;
	};
}

// ------------------------------------------------------------------
//
// @details This class is used to compare tasks for priority
//
// ------------------------------------------------------------------
class TaskCompare : public std::binary_function<const std::shared_ptr<const Tasks::Task>&, const std::shared_ptr<const Tasks::Task>&, bool>
{
public:
	bool operator()(const std::shared_ptr<const Tasks::Task>& lhs, const std::shared_ptr<const Tasks::Task>& rhs) const
	{
		return (lhs->getPriority() > rhs->getPriority());
	}
};

#endif // _TASK_HPP_
