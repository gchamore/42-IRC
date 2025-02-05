/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Constants.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchamore <gchamore@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/05 12:01:39 by gchamore          #+#    #+#             */
/*   Updated: 2025/02/05 12:03:16 by gchamore         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

namespace Constants
{
	static const size_t MAX_MESSAGE_LENGTH = 512;
	static const size_t MAX_PARAMS = 15;
	static const size_t MAX_PREFIX_LEN = 64;
	static const size_t MAX_COMMAND_LEN = 510;
	static const size_t MAX_CHANNEL_NAME_LENGTH = 50;
	static const size_t MAX_USERS_PER_CHANNEL = 50;
	static const size_t MAX_NICKNAME_LENGTH = 9;
	static const size_t MAX_USERNAME_LENGTH = 16;
	static const size_t MAX_HOSTNAME_LENGTH = 63;
}

#endif
