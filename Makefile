# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: gchamore <gchamore@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/01/16 15:30:31 by gchamore          #+#    #+#              #
#    Updated: 2025/01/16 15:32:59 by gchamore         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = ircserv

CC = c++

CFLAGS = -g -Wall -Wextra -Werror -std=c++98

RM = rm -rf

SRCS = main.cpp

OBJ_DIR = objs

OBJS = $(addprefix $(OBJ_DIR)/, $(SRCS:.cpp=.o))

DEPS = $(OBJS:.o=.d)

GREEN = \033[1;32m
YELLOW = \033[1;33m
ORANGE = \033[38;5;214m
RESET = \033[0m
CHECK_MARK = \033[1;32m✔\033[0m

all: first_header $(NAME)

$(NAME): $(OBJS)
	@$(CC) $(CFLAGS) -o $(NAME) $(OBJS) && printf "\n$(CHECK_MARK) $(GREEN)Compilation Succeeded$(RESET)\n\n\n"

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJS) $(OBJ_DIR)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re

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
 ╚═╝╚═╝  ╚═╝ ╚═════╝$(RESET) 

$(YELLOW)Internet Relay Chat$(RESET)
endef
export FIRST_HEADER

header:
	@echo "$$FIRST_HEADER"
