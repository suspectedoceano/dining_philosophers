NAME = philo
CC = gcc
NORM = norminette
CFLAGS = -g -O3 -Wall -Wextra -Werror
RM = rm -rf
OBJS_DIR = objs/

# Check for the operating system                                                   
# If it it LINUX i need to link pthread
ifeq ($(shell uname -s),Linux)
    CFLAGS += -pthread
endif

#Decide at compile time these values
#i.e. make DEBUG_MODE=1 PHILO_MAX=300
#if no values, using default DEBUG_MODE=0 and PHILO_MAX=200
#The -D flag in the context of the GCC compiler (and many other compilers) stands for "Define".
ifdef DEBUG_MODE
    CFLAGS += -DDEBUG_MODE=$(DEBUG_MODE) -fsanitize=thread 
endif

ifdef PHILO_MAX
    CFLAGS += -DPHILO_MAX=$(PHILO_MAX)
endif


SRCS = $(wildcard *.c)
OBJS = $(addprefix $(OBJS_DIR), $(SRCS:.c=.o))

all : $(OBJS_DIR) $(NAME)

$(OBJS_DIR) :
	mkdir -p $(OBJS_DIR)

$(OBJS_DIR)%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(NAME) : $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

re : fclean all

clean :
	$(RM) $(OBJS_DIR)

fclean : clean
	$(RM) $(NAME)

norm :
	@$(NORM) $(SRCS)

leaks: re
	@echo "\033[1;33m\nChecking for memory leaks...\033[0m"
	leaks --atExit -- ./$(NAME) 5 800 200 200 5

valgrind_race: re
	@echo "\033[1;33m\nChecking for race conditions with valgrind...\033[0m"
	valgrind --tool=helgrind ./$(NAME) 5 800 200 200 5

valgrind_leaks: re
	@echo "\033[1;33m\nChecking for memory leaks with valgrind...\033[0m"
	valgrind --leak-check=full ./$(NAME) 5 800 200 200 5


# Define symbolic constants for color codes
BOLD_CYAN=\033[1;36m
RESET_COLOR=\033[0m

help:
	@echo "Targets available:"
	@echo "  $(BOLD_CYAN)all$(RESET_COLOR)       : Compile the program"
	@echo "  $(BOLD_CYAN)clean$(RESET_COLOR)     : Remove object files"
	@echo "  $(BOLD_CYAN)fclean$(RESET_COLOR)    : Remove object files and the executable"
	@echo "  $(BOLD_CYAN)norm$(RESET_COLOR)      : Check the code with norminette"
	@echo "  $(BOLD_CYAN)leaks$(RESET_COLOR)     : Check the program for memory leaks"
	@echo "  $(BOLD_CYAN)valgrind_race$(RESET_COLOR)     : Check the program for race conditions in linux"
	@echo "  $(BOLD_CYAN)valgrind_leaks$(RESET_COLOR)     : Check the program for leaks  in linux"
	@echo ""
	@echo "Variables you can set:"
	@echo "  $(BOLD_CYAN)DEBUG_MODE$(RESET_COLOR) : Set to 1 to enable debugging mode (emoji + fsanitize=thread), just make fclean; make DEBUG_MODE=1"
	@echo "  $(BOLD_CYAN)PHILO_MAX$(RESET_COLOR)  : Set maximum number of philosophers (default is 200), just make fclean; make PHILO_MAX=your_value"
	@echo ""
	@echo "Example usage:"
	@echo "$(BOLD_CYAN)  make DEBUG_MODE=1, make PHILO_MAX=300$(RESET_COLOR)"


.PHONY : clean fclean re all bonus

