NAME = bot

SRCS =  main.cpp \
		Game.cpp \
		Bot.cpp \
		MCTS.cpp 
	
INCS = Bot.hpp Game.hpp  MCTS.hpp

ODIR = .objs/

OBJS = $(addprefix $(ODIR), $(SRCS:.cpp=.o))

CC = c++

FLAGS = -Wall -Wextra -Werror -std=c++98 #-fsanitize=address -g

all : $(NAME)

$(NAME) : $(OBJS) 
	$(CC) $(FLAGS) $(OBJS) -o $(NAME)

$(ODIR)%.o : %.cpp $(INCS)
	@mkdir -p $(dir $@)
	$(CC) $(FLAGS) -c $< -o $@ 

clean :
	rm -rf $(ODIR)

fclean : clean
	rm -rf $(NAME)

re : fclean all
