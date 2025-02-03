#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

namespace Constants
{
	static const size_t MAX_MESSAGE_LENGTH = 512;
	static const size_t MAX_PARAMS = 15;
	static const size_t MAX_PREFIX_LEN = 64;   // Changed from 512 to 64 (prefix length)
	static const size_t MAX_COMMAND_LEN = 510; // Length without CRLF
	static const size_t MAX_CHANNEL_NAME_LENGTH = 50;
	static const size_t MAX_USERS_PER_CHANNEL = 50;
}

#endif
