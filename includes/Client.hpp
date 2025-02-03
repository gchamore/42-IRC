/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anferre <anferre@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/17 17:58:01 by gchamore          #+#    #+#             */
/*   Updated: 2025/02/03 13:19:04 by anferre          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <stdexcept>
#include <iostream>
#include "Server.hpp"

class Server;
class Channel;

class Client
{

public:
	enum ClientState
	{
		REGISTERING,
		REGISTERED,
	};
	Client(int socket_fd);
	int getFD() const;
	const std::string &getNickname() const;
	void setNickname(const std::string &nick);
	const std::string &getUsername() const;
	void setUsername(const std::string &user);
	ClientState getState() const;
	void setState(ClientState newState);
	bool authenticated() const;
	void authenticate();
	void appendToBuffer(const std::string &data);
	bool hasCommand() const;
	std::string popCommand();
	void sendResponse(const std::string &response);
	std::vector<Channel *> getChannels(const Server &server) const;
	size_t getId() const { return userId; }
	// static void resetUserCount() { userCount = 0; }

private:
	int fd;					 // Descripteur de fichier / (socket)
	std::string nickname;	 // Pseudo de l'utilisateur
	std::string username;	 // Nom d'utilisateur
	bool isAuthenticated;	 // Indique si le client est authentifié ou non
	std::string buffer;		 // Buffer de réception
	ClientState state;		 // État du client
	static size_t userCount; // Compteur statique pour tous les clients
	size_t userId;			 // ID unique pour chaque client
};

#endif
