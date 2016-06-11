#ifndef _NEXTPRIMETASK_HPP_
#define _NEXTPRIMETASK_HPP_

#include "Shared/Messages/NextPrime.hpp"
#include "Task.hpp"

namespace Tasks
{
	// -----------------------------------------------------------------
	//
	// @details This class represents a "next prime" work unit.  It handles
	// the computation of the next prime number in a sequence.  Once that
	// is done, the result is returned back to the requesting source.
	//
	// -----------------------------------------------------------------
	class NextPrimeTask : public Task
	{
	public:
		NextPrimeTask(uint32_t lastPrime, Priority priority) :
			Task(priority),
			m_lastPrime(lastPrime),
			m_nextPrime(lastPrime)
		{
		}

		NextPrimeTask(std::shared_ptr<ip::tcp::socket> socket, Messages::NextPrime& message) :
			Task(socket, message.getTaskId())
		{
			m_lastPrime = message.m_message.lastprime();
			m_nextPrime = message.m_message.lastprime();
		}

		virtual void execute() override;

	protected:
		virtual std::shared_ptr<Messages::Message> getMessage() override;
		virtual std::shared_ptr<Messages::Message> completeCustom(boost::asio::io_service& ioService) override;

	private:
		uint32_t m_lastPrime;
		uint32_t m_nextPrime;

		bool isPrime(uint32_t value);
	};
}

#endif // _NEXTPRIMETASK_HPP_
