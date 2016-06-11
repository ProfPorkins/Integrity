#include "Message.hpp"

#include <array>
#include <chrono>
#include <iostream>
#include <iomanip>


namespace Messages
{
	// -----------------------------------------------------------------
	//
	// @details This function takes care of sending any message.  This
	// function is blocking.
	//
	// -----------------------------------------------------------------
	void send(std::shared_ptr<Message> message, std::shared_ptr<ip::tcp::socket> socket, std::function<void(bool)> onComplete)
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
				// Create an array to hold both the type and message size.  This can then be stuck into a single
				// buffer that is sent.
				std::array<uint8_t, 5> header;

				//
				// Start with the message type, a single byte
				header[0] = message->m_type[0];
				//
				// Next, send the size of the message.  The reason we do this is because the receiver
				// needs to create a std::vector of the appropriate size to receive the data.
				auto sendSize = message->getMessageSize();
				sendSize = htonl(static_cast<uint32_t>(sendSize));
				uint8_t* ptrSize = reinterpret_cast<uint8_t*>(&sendSize);
				header[1] = ptrSize[0];
				header[2] = ptrSize[1];
				header[3] = ptrSize[2];
				header[4] = ptrSize[3];

				//
				// If there is data in the message body, we'll create multiple buffers and send those, otherwise,
				// the message header IS the message and we'll send only that.
				if (sendSize > 0)
				{
					//
					// Using the free 'write' function to ensure all data is written before
					// blocking falls through.
					boost::asio::streambuf buffer;
					std::ostream os(&buffer);
					message->serializeToOstream(&os);
					{
						std::array<boost::asio::const_buffer, 2> bufs;
						bufs[0] = boost::asio::buffer(header);
						bufs[1] = buffer.data();

						auto size = boost::asio::write(*socket, bufs);
						buffer.consume(size - header.size());
					}
				}
				else
				{
					boost::asio::write(*socket, boost::asio::buffer(header));
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

	// -----------------------------------------------------------------
	//
	// @details This is a non-blocking method that allows code to post
	// a message to send over the selected socket.  The actual sending of
	// the message happens in the blocking send function above.
	//
	// -----------------------------------------------------------------
	void send(std::shared_ptr<Message> message, std::shared_ptr<ip::tcp::socket> socket, boost::asio::io_service& ioService, std::function<void(bool)> onComplete)
	{
		ioService.post(
			[message, socket, onComplete]()
			{
				send(message, socket, onComplete);
			});
	}

	// -----------------------------------------------------------------
	//
	// @details This method will ensure a send takes place on a specific strand.
	// In effect, this allows for a non-blocking send to be performed, while
	// ensuring all calls are queued and serviced on a single strand.  This
	// is intended for use on an io_service that has multiple threads and
	// working over different sockets.
	//
	// -----------------------------------------------------------------
	void send(std::shared_ptr<Message> message, const std::shared_ptr<ip::tcp::socket> socket, boost::asio::strand& strand, std::function<void(bool)> onComplete)
	{
		strand.post(
			[message, socket, onComplete]()
			{
				send(message, socket, onComplete);
			});
	}

	// -----------------------------------------------------------------
	//
	// @details This method takes care of receiving any message.
	//
	// -----------------------------------------------------------------
	void read(Message& message, std::shared_ptr<ip::tcp::socket> socket)
	{
		if (socket->is_open())
		{
			//
			// Need the exception handling in case the socket get closed while
			// we are sending.  If an exception occurs, we don't do anything because
			// the work retry will deal with the failure.
			try
			{
				//std::chrono::time_point<std::chrono::high_resolution_clock, std::chrono::nanoseconds> now = std::chrono::high_resolution_clock::now();
				//std::cout << "Before Reading" << std::fixed << std::setprecision(10) << (now.time_since_epoch().count() / 1000000000.0) << std::endl;

				//
				// Read the size of the rest of the message
				std::array<uint32_t, 1> size;
				boost::asio::read(*socket, boost::asio::buffer(size));
				size[0] = ntohl(size[0]);

				if (size[0] > 0)
				{
					boost::asio::streambuf buffer;

					// Use the free 'read' function to ensure all data is read before
					// blocking falls through.
					boost::asio::read(*socket, buffer.prepare(size[0]));
					buffer.commit(size[0]);

					std::istream is(&buffer);
					message.parseFromIstream(&is);
				}

				//now = std::chrono::high_resolution_clock::now();
				//std::cout << "After Reading" << std::fixed << std::setprecision(10) << (now.time_since_epoch().count() / 1000000000.0) << std::endl;
			}
			catch (std::exception)
			{
			}
		}
	}
}
