#include "IRange.hpp"

#include <array>
#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <iostream>
#include <mutex>
#include <thread>

void demoThreadMutex()
{
	uint16_t resource(0);
	std::mutex mutex;

	auto report = [](uint16_t value)
	{
		std::cout << "The value is: " << value << std::endl;
	};

	auto threadFunction =
		[&resource, &mutex, report](bool reportEvens)
		{
			while (resource < 10)
			{
				auto isEven = (resource % 2 == 0);
				if (isEven == reportEvens)
				{
					std::lock_guard<std::mutex> lock(mutex);
					resource++;
					report(resource);
				}
			}
		};

	std::thread thread1(threadFunction, true);
	std::thread thread2(threadFunction, false);

	thread1.join();
	thread2.join();
}

void demoAtomic()
{
	std::atomic<uint16_t> resource(0);

	auto report = [](uint16_t value)
	{
		std::cout << "The value is: " << value << std::endl;
	};

	auto threadFunction =
		[&resource, report](bool reportEvens)
		{
			while (resource < 10)
			{
				auto isEven = (resource % 2 == 0);
				if (isEven == reportEvens)
				{
					resource++;
					report(resource);
				}
			}
		};

	std::thread thread1(threadFunction, true);
	std::thread thread2(threadFunction, false);

	thread1.join();
	thread2.join();
}

template <typename T>
class Resource
{
public:
	Resource() : m_front(0), m_back(0), m_count(0) {}

	void add(T value)
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		// Have to wait until m_data has room
		m_isRoom.wait(lock,
			[this]()
			{
				// Verify there really is room to place
				// something...could be spurious wakeup.
				return m_count != m_data.size();
			});
		m_data[m_back] = value;
		m_back = (m_back + 1) % m_data.size();
		m_count++;
		// Signal the has data condition
		{
			std::unique_lock(std::mutex) lockNotify(m_mutex);
			m_isData.notify_one();
		}
	}

	T remove()
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		// Have to wait until m_data contains something
		m_isData.wait(lock,
			[this]()
			{
				// Verify there really is something to
				// work on...could be spurious wakeup.
				return m_count > 0;
			});
		T value = m_data[m_front];
		m_front = (m_front + 1) % m_data.size();
		m_count--;
		// Signal the has room condition
		{
			std::unique_lock(std::mutex) lockNotify(m_mutex);
			m_isData.notify_one();
		}

		return value;
	}

private:
	std::array<T, 10> m_data;
	std::uint8_t m_front;
	std::uint8_t m_back;
	std::uint8_t m_count;

	std::mutex m_mutex;
	std::condition_variable m_isRoom;
	std::condition_variable m_isData;
};

void demoConditionVariable()
{
	Resource<uint32_t> resources;

	auto producer =
		[&resources](uint8_t howMany)
		{
			for (auto item : IRange<uint8_t>(1, howMany))
			{
				resources.add(item);
				std::this_thread::sleep_for(std::chrono::milliseconds(50));
			}
		};

	auto consumer =
		[&resources](uint8_t howMany)
		{
			for (auto item : IRange<uint8_t>(1, howMany))
			{
				std::cout << "Thread: " << std::this_thread::get_id();
				std::cout << " Consumed: " << resources.remove();
				std::cout << std::endl;
				std::this_thread::sleep_for(std::chrono::milliseconds(200));
			}
		};

	std::thread producer1(producer, 100);
	std::thread consumer1(consumer, 25);
	std::thread consumer2(consumer, 25);
	std::thread consumer3(consumer, 25);
	std::thread consumer4(consumer, 25);

	producer1.join();
	consumer1.join();
	consumer2.join();
	consumer3.join();
	consumer4.join();
}

int main()
{
	std::cout << "-- demoThreadMutex --" << std::endl;
	demoThreadMutex();

	std::cout << std::endl << "-- demoAtomic --" << std::endl;
	demoAtomic();

	std::cout << std::endl << "-- demoConditionVariable --" << std::endl;
	demoConditionVariable();

	return 0;
}
