#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <algorithm>
#include <cstdio>
#include <fcntl.h>
#include <poll.h>
#include <vector>
#include <map>
#include <cstdlib>
#include "Client.hpp"
#include "Channel.hpp"
#include "CommandParser.hpp"
#include <sstream>

class Client;
class Channel;

class Server
{
private:
	int server_fd;
	int port;
	std::string server_password;
	std::vector<pollfd> poll_fds;
	std::map<int, Client *> clients;
	std::map<std::string, Channel *> channels;

	void setup_server();
	void accept_new_client();
	void handle_client_data(int client_fd);
	void remove_client(int client_fd);
	void broadcast_message(const std::string &channelName, const std::string &message);
	void delete_channel(const std::string &channelName);

	void handleCommand(const CommandParser::ParsedCommand &command, Client &client);
	void handlePassCommand(const CommandParser::ParsedCommand &command, Client &client);
	void handleNickCommand(const CommandParser::ParsedCommand &command, Client &client);
	bool isValidNickname(const std::string &nickname);
	bool isNicknameTaken(const std::string &nickname) const;
	void handleUserCommand(const CommandParser::ParsedCommand &command, Client &client);
	bool isValidUsername(const std::string &username);
	void handleJoinCommand(const CommandParser::ParsedCommand &command, Client &client);
	void handlePrivmsgCommand(const CommandParser::ParsedCommand &command, Client &client);
	void handlePartCommand(const CommandParser::ParsedCommand &command, Client &client);
	void handleQuitCommand(const CommandParser::ParsedCommand &command, Client &client);

public:
	Server(int port, const std::string &password);
	~Server();
	void start();
	std::vector<Channel *> getChannelsForClient(const Client *client) const;
};

#endif