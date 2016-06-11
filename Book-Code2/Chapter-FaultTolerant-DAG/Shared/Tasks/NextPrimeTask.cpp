#include "NextPrimeTask.hpp"

#include "Shared/Messages/NextPrime.hpp"
#include "Shared/Messages/NextPrimeResult.hpp"

#include <memory>
#include <thread>

namespace Tasks
{
	// -----------------------------------------------------------------
	//
	// @details The worst known algorithm for finding the next prime number :)
	//
	// -----------------------------------------------------------------
	void NextPrimeTask::execute()
	{
		m_nextPrime += 2;
		while (!isPrime(m_nextPrime))
		{
			m_nextPrime += 2;
		}

		//std::this_thread::sleep_for(std::chrono::milliseconds(2500));
	}

	// -----------------------------------------------------------------
	//
	// @details Builds the message used to indicate what work is to be done
	//
	// -----------------------------------------------------------------
	std::shared_ptr<Messages::Message> NextPrimeTask::getMessage()
	{
		return std::make_shared<Messages::NextPrime>(m_id, m_lastPrime);
	}

	// ------------------------------------------------------------------
	//
	// @details Prepare the result message to send back to the client
	//
	// ------------------------------------------------------------------
	std::shared_ptr<Messages::Message> NextPrimeTask::completeCustom(boost::asio::io_service& ioService)
	{
		//
		// Generate the message and return it to the calling code
		std::shared_ptr<Messages::NextPrimeResult> message = std::make_shared<Messages::NextPrimeResult>(this->getId(), m_nextPrime);
		return message;
	}

	// -----------------------------------------------------------------
	//
	// @details Test a value for the prime property.  Could use a little
	// work to eliminate all the returns, making a single point
	// of return.
	//
	// -----------------------------------------------------------------
	bool NextPrimeTask::isPrime(uint32_t value)
	{
		if (value == 2) return true;

		if (value % 2 == 0 || value <= 1) return false;

		for (int mod = 3; mod <= std::sqrt(value); mod += 2)
		{
			if (value % mod == 0)
			{
				return false;
			}
		}

		return true;
	}
}
