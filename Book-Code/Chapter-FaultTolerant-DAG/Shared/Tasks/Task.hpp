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
		Task(std::chrono::milliseconds duration);
		Task(std::shared_ptr<ip::tcp::socket> socket, uint32_t id);

		virtual ~Task() {}	// Virtual destructor to allow derived class destructors to correctly get called

		void send(std::shared_ptr<ip::tcp::socket> socket, boost::asio::strand& strand);
		virtual void execute() = 0;
		void complete(boost::asio::io_service& ioService);

		uint32_t getId()							{ return m_id; }
		std::chrono::milliseconds getDuration()		{ return m_duration; }

	protected:
		uint32_t m_id;
		std::shared_ptr<ip::tcp::socket> m_socket;

		virtual std::shared_ptr<Messages::Message> getMessage() = 0;
		virtual std::shared_ptr<Messages::Message> completeCustom(boost::asio::io_service& ioService) = 0;

	private:
		std::chrono::milliseconds m_duration;
	};
}

#endif // _TASK_HPP_
