NAME = server


SRCS =  srcs/server.cpp \
		srcs/utils.cpp \
		srcs/signals.cpp \
		srcs/sockets.cpp \
		srcs/communication.cpp \
		srcs/steps.cpp \
		srcs/parsing.cpp


INCS = include

ODIR = objs/

OBJS = $(addprefix $(ODIR), $(SRCS:.cpp=.o))
# FULL_SRCS_PATH = $(addprefix $(SRCSDIR), $(SRCS))
CC = c++

FLAGS = -Wall -Wextra -std=c++98 #-Werror

all : $(NAME)

$(NAME) : $(OBJS)
	$(CC) $(FLAGS) $(OBJS) -o $(NAME)

$(ODIR)%.o : %.cpp $(INCS)
	mkdir -p $(dir $@)
	$(CC) $(FLAGS) -c $< -o $@ -I$(INCS)

clean :
	rm -rf $(ODIR)

fclean : clean
	rm -rf $(NAME)

re : fclean all