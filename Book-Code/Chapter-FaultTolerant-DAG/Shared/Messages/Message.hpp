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

#ifndef _MESSAGE_HPP_
#define _MESSAGE_HPP_

#include "MessageTypes.hpp"

#include <array>
#include <functional>
#include <string>

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
	class Message : public std::enable_shared_from_this<Message>
	{
	public:
		Message(Type type) :
			m_decodePosition(0)
		{
			m_type[0] = static_cast<uint8_t>(type);
		}

		virtual ~Message() {}	// Virtual destructor to allow derived destructors to be called

		Type getType() { return static_cast<Type>(m_type[0]); }

		void send(std::shared_ptr<ip::tcp::socket> socket, std::function<void(bool)> onComplete = [](bool) {});
		void send(std::shared_ptr<ip::tcp::socket> socket, boost::asio::strand& strand, std::function<void(bool)> onComplete = [](bool) {});

		void read(std::shared_ptr<ip::tcp::socket> socket);

	protected:
		virtual void encodeMessage() = 0;
		virtual void decodeMessage() = 0;

		void encode(uint8_t value);
		void encode(uint16_t value);
		void encode(uint32_t value);
		void encode(double value);
		void encode(std::string value);

		void decode(uint8_t& value);
		void decode(uint16_t& value);
		void decode(uint32_t& value);
		void decode(double& value);
		void decode(std::string& value);

	private:
		std::array<uint8_t, 1> m_type;
		std::vector<uint8_t> m_data;
		uint16_t m_decodePosition;
	};
}

#endif // _MESSAGE_HPP_
