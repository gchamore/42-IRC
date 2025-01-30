# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: anferre <anferre@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/01/16 15:30:31 by gchamore          #+#    #+#              #
#    Updated: 2025/01/29 13:11:51 by anferre          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = ircserv

CC = c++

CFLAGS = -g3 -Wall -Wextra -Werror -std=c++98

RM = rm -rf

SRCS = srcs/main.cpp \
       srcs/Client.cpp \
       srcs/Channel.cpp \
       srcs/CommandParser.cpp \
	   srcs/Server.cpp \
	   srcs/ServerCommands.cpp \
	   srcs/OperatorCommands.cpp \

OBJ_DIR = objs

OBJS = $(patsubst srcs/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))

DEPS = $(OBJS:.o=.d)

GTEST_DIR = /usr/local/gtest

INCLUDES = -I$(GTEST_DIR)/include

LIBS = -L$(GTEST_DIR)/lib -lgtest -lgtest_main -pthread

GREEN = \033[1;32m
YELLOW = \033[1;33m
ORANGE = \033[38;5;214m
RESET = \033[0m
CHECK_MARK = \033[1;32m✔\033[0m

all: first_header $(NAME)

$(NAME): $(OBJS)
	@$(CC) $(CFLAGS) -o $(NAME) $(OBJS) && printf "\n$(CHECK_MARK) $(GREEN)Compilation Succeeded$(RESET)\n\n\n"

$(OBJ_DIR)/%.o: srcs/%.cpp
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@


clean:
	$(RM) $(OBJS) $(OBJ_DIR)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re test

info: header

-include $(DEPS)

first_header:
	@echo "\n $$FIRST_HEADER \n"

define FIRST_HEADER
$(ORANGE)██╗██████╗  ██████╗
 ██║██╔══██╗██╔════╝
 ██║██████╔╝██║     
 ██║██╔══██╗██║     
 ██║██║  ██║╚██████╗
 ╚═╝╚═╝  ╚═════╝$(RESET) 

$(YELLOW)Internet Relay Chat$(RESET)
endef
export FIRST_HEADER

header:
	@echo "$$FIRST_HEADER"
