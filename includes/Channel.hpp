/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchamore <gchamore@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/17 17:57:59 by gchamore          #+#    #+#             */
/*   Updated: 2025/01/23 16:20:00 by gchamore         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <vector>
#include "Client.hpp"

class Channel
{
private:
    std::string name;
    std::vector<Client*> members;

public:
    Channel();
    Channel(const std::string& channelName);
    void addMember(Client* client);
    void removeMember(Client* client);
    const std::vector<Client*>& getMembers() const;
    const std::string& getName() const;
};

#endif

