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
