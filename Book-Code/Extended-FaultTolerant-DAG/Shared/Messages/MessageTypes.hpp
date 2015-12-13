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

#ifndef _MESSAGETYPES_HPP_
#define _MESSAGETYPES_HPP_

#include <cstdint>
#include <unordered_map>

namespace Messages
{
	// -----------------------------------------------------------------
	//
	// @details These are the types of messages that can be sent throughout
	// the system.
	//
	// -----------------------------------------------------------------
	enum class Type : uint8_t
	{
		Undefined,
		TaskRequest,
		MandelMessage,
		MandelResult,
		MandelFinished,
		MandelFinishedResult,
		NextPrime,
		NextPrimeResult,
		DAGExample,
		DAGExampleResult,
		TerminateCommand,
		TaskStatus
	};
}


namespace std
{
	// -----------------------------------------------------------------
	//
	// @details This provides a std::hash for the Messages::Type enum so
	// that it can be used in a std::unordered_map.
	// Found this code on Stack Overflow at: http://stackoverflow.com/questions/18837857/cant-use-enum-class-as-unordered-map-key
	//
	// -----------------------------------------------------------------
	template<>
	struct hash<Messages::Type>
	{
	typedef Messages::Type argument_type;
	typedef std::underlying_type<argument_type>::type underlying_type;
	typedef std::hash<underlying_type>::result_type result_type;
	result_type operator()(const argument_type& arg) const
	{
		std::hash<underlying_type> hasher;
		return hasher(static_cast<underlying_type>(arg));
	}
	};
}

#endif // _MESSAGE_HPP_
