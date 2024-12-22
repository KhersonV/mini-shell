# Имя исполняемого файла будет определяться после BUILD_DIR
DEBUG = 1
ifeq ($(DEBUG), 1)
    CFLAGS = -g -Wall -Wextra -Werror
    BUILD_DIR = build/Debug
else
    CFLAGS = -Wall -Wextra -Werror
    BUILD_DIR = build/Release
endif

NAME = $(BUILD_DIR)/minishell

# Компилятор
CC = gcc

# Папки
SRC_DIR = src
OBJ_DIR = $(BUILD_DIR)/obj
LIBFT_DIR = libft

# Источники
SRC = $(SRC_DIR)/main.c \
      $(SRC_DIR)/builtins/cd.c $(SRC_DIR)/builtins/echo.c $(SRC_DIR)/builtins/env_builtin.c \
      $(SRC_DIR)/builtins/exit_builtin.c $(SRC_DIR)/builtins/export.c $(SRC_DIR)/builtins/pwd.c \
      $(SRC_DIR)/env/env.c $(SRC_DIR)/env/env_utils.c \
      $(SRC_DIR)/executor/executor.c $(SRC_DIR)/executor/executor_utils.c \
      $(SRC_DIR)/executor/herodoc.c $(SRC_DIR)/executor/pipes.c $(SRC_DIR)/executor/redirection.c \
      $(SRC_DIR)/lexer/lexer.c $(SRC_DIR)/lexer/lexer_utils.c $(SRC_DIR)/lexer/tokenizer.c \
      $(SRC_DIR)/parser/ast.c $(SRC_DIR)/parser/parser.c $(SRC_DIR)/parser/parser_utils.c \
      $(SRC_DIR)/signals/signals.c $(SRC_DIR)/utils/errors.c $(SRC_DIR)/utils/free_utils.c 

# Преобразование исходников в объектные файлы с использованием patsubst
OBJ = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))

# Библиотеки
LIBFT = $(LIBFT_DIR)/libft.a

# Основная цель
all: $(NAME)

# Сборка исполняемого файла
$(NAME): $(OBJ) $(LIBFT)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(OBJ) $(LIBFT) $(LDFLAGS) -lreadline -o $@

# Компиляция объектных файлов
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Сборка библиотеки libft
$(LIBFT):
	$(MAKE) -C $(LIBFT_DIR)

# Очистка объектных файлов и директории сборки
clean:
	rm -rf $(BUILD_DIR)
	$(MAKE) clean -C $(LIBFT_DIR)

# Полная очистка (включая исполняемый файл)
fclean: clean
	rm -f $(NAME)
	$(MAKE) fclean -C $(LIBFT_DIR)

# Пересборка
re: fclean all

# Объявление phony целей
.PHONY: all clean fclean re
