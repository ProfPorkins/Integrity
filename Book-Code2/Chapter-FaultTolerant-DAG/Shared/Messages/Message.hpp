#ifndef _MESSAGE_HPP_
#define _MESSAGE_HPP_

#include "MessageTypes.hpp"

#include <array>
#include <functional>
#include <ostream>

//
// Disable some compiler warnings that come from boost
#pragma warning(push)
#pragma warning(disable : 4267)
#pragma warning(disable : 4996)
#include <boost/asio.hpp>
#pragma warning(pop)

namespace Messages
{
	namespace ip = boost::asio::ip;

	// -----------------------------------------------------------------
	//
	// @details This is the base message class from which all other messages
	// are derived.
	//
	// -----------------------------------------------------------------
	class Message
	{
	public:
		Message(Type type)
		{
			m_type[0] = static_cast<uint8_t>(type);
		}

		virtual ~Message() {}	// Virtual destructor to allow derived destructors to be called

	private:
		friend void send(std::shared_ptr<Message> message, std::shared_ptr<ip::tcp::socket> socket, std::function<void(bool)> onComplete);
		friend void read(Message& message, std::shared_ptr<ip::tcp::socket> socket);

		virtual uint32_t getMessageSize() = 0;
		virtual bool serializeToOstream(std::ostream* output) const = 0;
		virtual bool parseFromIstream(std::istream* input) = 0;

	private:
		std::array<uint8_t, 1> m_type;
	};

	void send(std::shared_ptr<Message> message, std::shared_ptr<ip::tcp::socket> socket, boost::asio::io_service& ioService, std::function<void(bool)> onComplete = [](bool) {});
	void send(std::shared_ptr<Message> message, const std::shared_ptr<ip::tcp::socket> socket, boost::asio::strand& strand, std::function<void(bool)> onComplete = [](bool) {});

	void read(Message& message, std::shared_ptr<ip::tcp::socket> socket);
}

#endif // _MESSAGE_HPP_
