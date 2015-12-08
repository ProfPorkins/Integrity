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

#include "Message.hpp"

namespace Messages
{
	// ------------------------------------------------------------------
	//
	// @details This method takes care of sending any message, derived
	// classes do not need to implement any sending code.
	//
	// ------------------------------------------------------------------
	void Message::send(std::shared_ptr<ip::tcp::socket> socket, std::function<void(bool)> onComplete)
	{
		if (socket->is_open())
		{
			//
			// Now that we are about to send, go ahead and encode.
			this->encodeMessage();
			//
			// Need the exception handling in case the socket get closed while
			// we are sending.  If an exception occurs, we don't do anything because
			// the work retry will deal with the failure.
			try
			{
				//
				// Start by sending the message type
				boost::asio::write(*socket, boost::asio::buffer(m_type));
				//
				// Next, send the size of the message.  The reason we do this is because the receiver
				// needs to create a std::vector of the appropriate size to receive the data.
				std::array<uint16_t, 1> size;
				size[0] = htons(static_cast<uint16_t>(m_data.size()));
				boost::asio::write(*socket, boost::asio::buffer(size));

				//
				// Finish by sending the message data, if any
				if (m_data.size() > 0)
				{
					//
					// Using the free 'write' function to ensure all data is written before
					// blocking falls through.
					boost::asio::write(
						*socket,
						boost::asio::buffer(m_data));
				}
				//
				// Invoke the callback passed in to let calling code know the send has completed.
				onComplete(true);
			}
			catch (std::exception)
			{
				onComplete(false);
			}
		}
	}

	// ------------------------------------------------------------------
	//
	// @details This method will ensure a send takes place on a specific strand.
	// If effect, this allows for a non-blocking send to be performed, while
	// ensuring all calls are queued and serviced on a single strand.  This
	// is intended for use on an io_service that has multiple threads.
	//
	// ------------------------------------------------------------------
	void Message::send(std::shared_ptr<ip::tcp::socket> socket, boost::asio::strand& strand, std::function<void(bool)> onComplete)
	{
		auto captureThis = shared_from_this();
		strand.post(
			[captureThis, socket, onComplete]()
			{
				captureThis->send(socket, onComplete);
			});
	}

	// ------------------------------------------------------------------
	//
	// @details This method takes care of receiving sending any message.
	// Derived classes will have implemented their own decodeMessage
	// method, which this code calls.
	//
	// ------------------------------------------------------------------
	void Message::read(std::shared_ptr<ip::tcp::socket> socket)
	{
		if (socket->is_open())
		{
			//
			// Need the exception handling in case the socket get closed while
			// we are sending.  If an exception occurs, we don't do anything because
			// the work retry will deal with the failure.
			try
			{
				//
				// Read the size of the rest of the message
				std::array<uint16_t, 1> size;
				boost::asio::read(*socket, boost::asio::buffer(size));
				size[0] = ntohs(size[0]);
				m_data.resize(size[0]);
				if (size[0] > 0)
				{
					//
					// We already know the message type, now only need to read and decode
					// the data.  Use the free 'read' function to ensure all data is read before
					// blocking falls through.
					boost::asio::read(*socket, boost::asio::buffer(m_data));
					this->decodeMessage();
				}
			}
			catch(std::exception)
			{
			}
		}
	}

	void Message::encode(uint8_t value)
	{
		m_data.push_back(value);
	}

	void Message::encode(uint16_t value)
	{
		uint16_t network = htons(value);
		uint8_t* bytes = reinterpret_cast<uint8_t*>(&network);
		m_data.push_back(bytes[0]);
		m_data.push_back(bytes[1]);
	}

	void Message::encode(uint32_t value)
	{
		uint32_t network = htonl(value);
		uint8_t* bytes = reinterpret_cast<uint8_t*>(&network);
		m_data.push_back(bytes[0]);
		m_data.push_back(bytes[1]);
		m_data.push_back(bytes[2]);
		m_data.push_back(bytes[3]);
	}

	void Message::encode(double value)
	{
		uint8_t* bytes = reinterpret_cast<uint8_t*>(&value);
		m_data.push_back(bytes[0]);
		m_data.push_back(bytes[1]);
		m_data.push_back(bytes[2]);
		m_data.push_back(bytes[3]);
		m_data.push_back(bytes[4]);
		m_data.push_back(bytes[5]);
		m_data.push_back(bytes[6]);
		m_data.push_back(bytes[7]);
	}


	void Message::decode(uint8_t& value)
	{
		value = m_data[m_decodePosition++];
	}

	void Message::decode(uint16_t& value)
	{
		uint8_t* bytes = reinterpret_cast<uint8_t*>(&value);
		bytes[0] = m_data[m_decodePosition++];
		bytes[1] = m_data[m_decodePosition++];

		value = ntohs(value);
	}

	void Message::decode(uint32_t& value)
	{
		uint8_t* bytes = reinterpret_cast<uint8_t*>(&value);
		bytes[0] = m_data[m_decodePosition++];
		bytes[1] = m_data[m_decodePosition++];
		bytes[2] = m_data[m_decodePosition++];
		bytes[3] = m_data[m_decodePosition++];

		value = ntohl(value);
	}

	void Message::decode(double& value)
	{
		uint8_t* bytes = reinterpret_cast<uint8_t*>(&value);
		bytes[0] = m_data[m_decodePosition++];
		bytes[1] = m_data[m_decodePosition++];
		bytes[2] = m_data[m_decodePosition++];
		bytes[3] = m_data[m_decodePosition++];
		bytes[4] = m_data[m_decodePosition++];
		bytes[5] = m_data[m_decodePosition++];
		bytes[6] = m_data[m_decodePosition++];
		bytes[7] = m_data[m_decodePosition++];
	}
}
