/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchamore <gchamore@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/17 17:58:01 by gchamore          #+#    #+#             */
/*   Updated: 2025/01/23 16:19:51 by gchamore         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <stdexcept>
#include <iostream>

class Client
{
private:
    int fd;                     // Descripteur de fichier / (socket)
    std::string nickname;       // Pseudo de l'utilisateur
    std::string username;       // Nom d'utilisateur
    bool isAuthenticated;       // Indique si le client est authentifié ou non
    std::string buffer;        // Buffer de réception

public:
    Client(int socket_fd);
    int getFD() const;
    const std::string& getNickname() const;
    void setNickname(const std::string& nick);
    const std::string& getUsername() const;
    void setUsername(const std::string& user);
    bool authenticated() const;
    void authenticate(const std::string& server_password, const std::string& password);
    void appendToBuffer(const std::string& data);
    bool hasCommand() const;
    std::string popCommand();
    void sendResponse(const std::string& response);
};

#endif
