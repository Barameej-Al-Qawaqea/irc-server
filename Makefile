# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: yrhiba <yrhiba@student.1337.ma>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/06/07 18:15:39 by yrhiba            #+#    #+#              #
#    Updated: 2024/06/08 22:22:20 by yrhiba           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

####### yrhiba ##########
#########################

NAME = server

SRCS =	server.cpp \
		utils.cpp \
		signals.cpp \
		sockets.cpp \
		communication.cpp \
		steps.cpp \
		parsing.cpp

INCS = header.hpp

ODIR = objs/

OBJS = $(addprefix $(ODIR), $(SRCS:.cpp=.o))

CC = c++

FLAGS = -Wall -Wextra -std=c++17 #-Werror 

$(NAME) : $(OBJS)
	$(CC) $(FLAGS) $(OBJS) -o $(NAME)

$(ODIR)%.o : %.cpp $(INCS)
	mkdir -p $(dir $@)
	$(CC) $(FLAGS) -c $< -o $@

all : $(NAME)

clean :
	rm -rf $(ODIR)

fclean : clean
	rm -rf $(NAME)

re : fclean all

.PHONY : all clean fclean re
