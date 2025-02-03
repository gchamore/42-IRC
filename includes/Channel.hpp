/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchamore <gchamore@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/17 17:57:59 by gchamore          #+#    #+#             */
/*   Updated: 2025/01/30 15:30:56 by gchamore         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <vector>
#include <map>
#include <set>
#include "Client.hpp"
#include <ctime>

class Client;

class Channel
{
private:
    std::string name;
    Client* creator;  // Createur du channel
	bool inviteOnly;    				// +i: invite only mode 
	std::string topic;
	bool topicRestricted;				// +t: topic restricted mode
	int userLimit;						// +l: user limit
	std::string password;				// +k: password
    std::map<Client*, bool> operators;	// List of operators
	std::set<Client *> inviteList;
    std::vector<Client*> members;
    time_t lastModified;  // Timestamp de la dernière modification

public:
    Channel();
    Channel(const std::string& channelName, Client* channelCreator);
	
	//channel methods
    void addMember(Client* client);
    void removeMember(Client* client);
	bool isMember(Client* client) const;
    const std::vector<Client*>& getMembers() const;
	
	//invite command methods
	bool isInviteOnly() const;
	void addInvite(Client* client);
	void removeInvite(Client* client);
	bool isInvited(Client* client) const;
	void setInviteOnly(bool inviteOnly);
	
	//operator methods
    const std::string& getName() const;
    bool isOperator(Client* client) const;
    void addOperator(Client* client);
    void removeOperator(Client* client);
	void sendTopic(Client* client) const;
	void sendUserList(Client* client) const;
	
	//topic command methods
	const std::string& getTopic() const;
	void setTopic(const std::string &topic);
	bool hasTopic() const;
	void setTopicRestricted(bool restricted);
	bool isTopicRestricted() const;

	//password command methods
	void setPassword(const std::string &password);
	bool hasPassword() const;
	const std::string &getPassword() const;
	void removePassword();

	//user limit command methods
	void setUserLimit(int limit);
	int getUserLimit() const;
	bool isFull() const;

    // cache methods
    time_t getLastModified() const { return lastModified; }
    void updateLastModified() { lastModified = time(NULL); }
};

#endif

