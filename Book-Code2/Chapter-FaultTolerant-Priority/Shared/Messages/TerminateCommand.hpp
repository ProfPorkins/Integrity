#ifndef _TERMINATECOMMAND_HPP_
#define _TERMINATECOMMAND_HPP_

#include "MessagePBMixIn.hpp"

//
// Google Protocol Buffers cause hella warnings, ignore them
#pragma warning(push, 0)
#include "TerminateCommand.pb.h"
#pragma warning(pop)

namespace Messages
{
	// -----------------------------------------------------------------
	//
	// @details This message is used to tell the compute server to terminate its process.
	//
	// -----------------------------------------------------------------
	class TerminateCommand : public MessagePBMixIn<PBMessages::TerminateCommand>
	{
	public:
		TerminateCommand() :
			MessagePBMixIn(Messages::Type::TerminateCommand)
		{
		}
	};
}

#endif // _TERMINATECOMMAND_HPP_
