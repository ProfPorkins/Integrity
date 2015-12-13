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
