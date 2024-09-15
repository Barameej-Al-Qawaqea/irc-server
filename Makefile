NAME = ircserv


SRCS =  srcs/server.cpp \
		srcs/utils.cpp \
		srcs/signals.cpp \
		srcs/sockets.cpp \
		srcs/communication.cpp \
		srcs/steps.cpp \
		srcs/ChannelCommands.cpp \
		srcs/parsing.cpp \
		srcs/commands.cpp \
		srcs/Channel.cpp \
		srcs/Client.cpp \
		srcs/authenticationCommands.cpp \
		srcs/PRIVMSGcommand.cpp \
		Bot/Bot.cpp \
		Bot/Game.cpp \
		Bot/MCTS.cpp

INCS = include

ODIR = .objs/


HEADERS = include/Channel.hpp \
		include/Client.hpp \
		include/Commands.hpp \
		include/header.hpp \
		include/Replies.hpp \
		include/Server.hpp \
		Bot/Bot.hpp \
		Bot/Game.hpp \
		Bot/MCTS.hpp

OBJS = $(addprefix $(ODIR), $(SRCS:.cpp=.o))

CC = c++

FLAGS = -Wall -Wextra -Werror -std=c++98

all : $(NAME)

$(NAME) : $(OBJS) 
	$(CC) $(FLAGS) $(OBJS) -o $(NAME)

$(ODIR)%.o : %.cpp $(HEADERS)
	@mkdir -p $(dir $@)
	$(CC) $(FLAGS) -c $< -o $@ -I$(INCS)

test : all
	c++ -I include -I Bot -I /opt/homebrew/opt/googletest/include -L /opt/homebrew/opt/googletest/lib -lgtest -lgtest_main -pthread -std=c++98 tests/*.cpp $(OBJS) -o test

clean :
	rm -rf $(ODIR)

fclean : clean
	rm -rf $(NAME)

re : fclean all
