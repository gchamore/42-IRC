
#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <cstdio>
#include <fcntl.h>
#include <poll.h>
#include <vector>
#include <map>
#include <cstdlib>
#include "Client.hpp"

void setup_server(int &server_fd, int port);
void server(int server_fd, std::vector<pollfd> poll_fds, std::string server_password);

#endif