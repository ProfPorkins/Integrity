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

#ifndef _TASK_HPP_
#define _TASK_HPP_

#include "Shared/Messages/Message.hpp"

#include <chrono>
#include <functional>
#include <memory>

//
// Disable some compiler warnings that come from boost
#pragma warning(push)
#pragma warning(disable : 4267)
#pragma warning(disable : 4996)
#include <boost/asio.hpp>
#pragma warning(pop)

namespace ip = boost::asio::ip;

namespace Tasks
{
	// ------------------------------------------------------------------
	//
	// @details This is the base class from which all tasks are derived.
	//
	// ------------------------------------------------------------------
	class Task
	{
	public:
		enum class Priority : uint8_t
		{
			One = 1,
			Two = 2,
			Three = 3
		};

		Task(std::chrono::milliseconds duration, Priority priority = Priority::One);
		Task(std::shared_ptr<ip::tcp::socket> socket, uint32_t id, Priority priority = Priority::One);

		virtual ~Task() {}	// Virtual destructor to allow derived class destructors to correctly get called

		void send(std::shared_ptr<ip::tcp::socket> socket, boost::asio::strand& strand);
		virtual void execute() = 0;
		void complete(boost::asio::io_service& ioService);

		uint32_t getId() const							{ return m_id; }
		Priority getPriority() const					{ return m_priority; }
		void setPriority(Priority priority)				{ m_priority = priority; }
		std::chrono::milliseconds getDuration() const	{ return m_duration; }

	protected:
		uint32_t m_id;
		std::shared_ptr<ip::tcp::socket> m_socket;

		virtual std::shared_ptr<Messages::Message> getMessage() = 0;
		virtual std::shared_ptr<Messages::Message> completeCustom(boost::asio::io_service& ioService) = 0;

	private:
		Priority m_priority;
		std::chrono::milliseconds m_duration;
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
		return (lhs->getPriority() < rhs->getPriority());
	}
};

#endif // _TASK_HPP_
