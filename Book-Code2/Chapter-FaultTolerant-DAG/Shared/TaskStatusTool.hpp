#ifndef _TASKSTATUSTOOL_HPP_
#define _TASKSTATUSTOOL_HPP_

#include <atomic>
#include <chrono>
#include <memory>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <vector>

//
// Disable some compiler warnings that come from boost
#pragma warning(push)
#pragma warning(disable : 4267)
#pragma warning(disable : 4996)
#include <boost/asio.hpp>
#pragma warning(pop)

namespace ip = boost::asio::ip;

class TaskStatusTool
{
public:
	static TaskStatusTool* instance();

	void initialize(boost::asio::io_service* ioService, std::shared_ptr<ip::tcp::socket>);
	static void terminate();
	void addTask(uint64_t taskId);
	void removeTask(uint64_t taskId);

protected:
	TaskStatusTool() : m_done(false)
	{
	}

private:
	static std::shared_ptr<TaskStatusTool> m_instance;

	struct Tracking
	{
		uint64_t taskId;
		std::chrono::milliseconds elapsedTime;
	};

	boost::asio::io_service* m_ioService;
	std::shared_ptr<ip::tcp::socket> m_socket;

  std::atomic<bool> m_done;
	std::shared_ptr<std::thread> m_statusThread;

	std::unordered_map<uint64_t, Tracking> m_activeTasks;
	std::unordered_map<uint64_t, Tracking> m_addedTasks;
	std::vector<uint64_t> m_removedTasks;
	std::mutex m_mutexActiveTasks;
	std::mutex m_mutexAddedTasks;
	std::mutex m_mutexRemovedTasks;

	void updateTasks(std::chrono::milliseconds elapsedTime);
};

#endif // _TASKSTATUSTOOL_HPP_
