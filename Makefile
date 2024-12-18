NAME = minishell

# Компилятор и флаги
CC = gcc
CFLAGS = -Wall -Wextra -Werror

# Директории
SRC_DIR = src
OBJ_DIR = obj
INC_DIR = include

# Указание библиотеки libft
LIBFT = libft/libft.a

# Указание всех исходников проекта
SRC = $(wildcard $(SRC_DIR)/**/*.c)

# Преобразование исходников в объектные файлы
OBJ = $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Основная цель
all: $(NAME)

# Сборка исполняемого файла
$(NAME): $(OBJ) $(LIBFT)
	$(CC) $(CFLAGS) $(OBJ) $(LIBFT) -o $(NAME)

# Компиляция объектных файлов
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I$(INC_DIR) -Ilibft -c $< -o $@

# Сборка libft
$(LIBFT):
	make -C libft

# Очистка объектных файлов
clean:
	rm -rf $(OBJ_DIR)
	make clean -C libft

# Полная очистка
fclean: clean
	rm -f $(NAME)
	make fclean -C libft

# Пересобрать проект
re: fclean all
