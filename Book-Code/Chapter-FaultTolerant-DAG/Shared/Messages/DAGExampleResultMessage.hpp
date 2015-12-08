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

#ifndef _DAGEXAMPLERESULTMESSAGE_HPP_
#define _DAGEXAMPLERESULTMESSAGE_HPP_

#include "ResultMessage.hpp"

namespace Messages
{
	// -----------------------------------------------------------------
	//
	// @details This is used to send the DAG Example result notification
	// back to the client.
	//
	// -----------------------------------------------------------------
	class DAGExampleResultMessage : public ResultMessage
	{
	public:
		DAGExampleResultMessage() :
			ResultMessage(Messages::Type::DAGExampleResult)
		{
		}

		DAGExampleResultMessage(uint32_t taskId, std::string name) :
			ResultMessage(Messages::Type::DAGExampleResult, taskId),
			m_name(name)
		{
		}

		std::string getName() { return m_name; }

	protected:
		virtual void encodeResult() override;
		virtual void decodeResult() override;

	private:
		std::string m_name;
	};
}

#endif // _DAGEXAMPLERESULTMESSAGE_HPP_
