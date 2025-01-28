/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchamore <gchamore@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/17 17:57:59 by gchamore          #+#    #+#             */
/*   Updated: 2025/01/27 14:32:33 by gchamore         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <vector>
#include <map>
#include "Client.hpp"

class Client;

class Channel
{
private:
    std::string name;
    std::vector<Client*> members;
    std::map<Client*, bool> operators; // Map pour privileges
    Client* creator;  // Createur du channel

public:
    Channel();
    Channel(const std::string& channelName, Client* channelCreator);
    void addMember(Client* client);
    void removeMember(Client* client);
    bool isOperator(Client* client) const;
    const std::vector<Client*>& getMembers() const;
    const std::string& getName() const;
    void addOperator(Client* client);
    void removeOperator(Client* client);
};

#endif

