NAME = minishell

CC = gcc
CFLAGS = -Wall -Wextra -Werror
SRC = src/main.c src/retranslate.c src/builtins.c
OBJ = $(SRC:.c=.o)
LIBFT = libft/libft.a
INC = -Iinclude

all: $(NAME)

$(NAME): $(OBJ) $(LIBFT)
	$(CC) $(CFLAGS) $(OBJ) $(LIBFT) -lreadline -o $(NAME)

%.o: %.c
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

$(LIBFT):
	make -C libft

clean:
	rm -f $(OBJ)
	make clean -C libft

fclean: clean
	rm -f $(NAME)
	make fclean -C libft

re: fclean all