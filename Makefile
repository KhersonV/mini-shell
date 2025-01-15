# Имя исполняемого файла
DEBUG = 1
ifeq ($(DEBUG), 1)
    CFLAGS = -g -Wall -Wextra -Werror  
    # -fsanitize=address
    BUILD_DIR = build/
else
    CFLAGS =  -Wall -Wextra -Werror
    BUILD_DIR = build/
endif

NAME = minishell

# Компилятор
CC = gcc

# Папки
SRC_DIR = src
OBJ_DIR = $(BUILD_DIR)/obj
LIBFT_DIR = libft
GNL_DIR = get_next_line

# Источники
SRC = $(SRC_DIR)/main.c \
      $(SRC_DIR)/builtins/cd.c $(SRC_DIR)/builtins/echo.c $(SRC_DIR)/builtins/env_builtin.c \
      $(SRC_DIR)/builtins/exit_builtin.c $(SRC_DIR)/builtins/export.c $(SRC_DIR)/builtins/pwd.c \
      $(SRC_DIR)/builtins/unset.c $(SRC_DIR)/builtins/cd_utils.c $(SRC_DIR)/builtins/cd_utils2.c \
      $(SRC_DIR)/env/env.c $(SRC_DIR)/env/env_utils.c $(SRC_DIR)/builtins/export_utils.c \
      $(SRC_DIR)/env/env_utils3.c $(SRC_DIR)/env/env_utils2.c $(SRC_DIR)/executor/executor_utils2.c \
      $(SRC_DIR)/executor/executor.c $(SRC_DIR)/executor/executor_utils.c $(SRC_DIR)/executor/pipes_utils.c \
      $(SRC_DIR)/executor/herodoc.c $(SRC_DIR)/executor/pipes.c $(SRC_DIR)/executor/executor_utils3.c \
      $(SRC_DIR)/lexer/lexer.c $(SRC_DIR)/lexer/lexer_utils.c $(SRC_DIR)/lexer/expansion.c \
      $(SRC_DIR)/lexer/syntax_check.c $(SRC_DIR)/lexer/lexer_utils2.c $(SRC_DIR)/lexer/lexer_utils3.c \
      $(SRC_DIR)/lexer/lexer_utils4.c $(SRC_DIR)/lexer/lexer_utils5.c \
      $(SRC_DIR)/lexer/lexer_utils6.c $(SRC_DIR)/lexer/lexer_utils7.c $(SRC_DIR)/lexer/lexer_utils8.c \
      $(SRC_DIR)/lexer/syntax_check2.c\
      $(SRC_DIR)/parser/parser.c $(SRC_DIR)/parser/parser2.c \
      $(SRC_DIR)/signals/signals.c $(SRC_DIR)/signals/signal_mode.c \
      $(SRC_DIR)/utils/errors.c $(SRC_DIR)/utils/free_utils.c $(SRC_DIR)/utils/free_utils2.c\
      $(GNL_DIR)/get_next_line_utils.c $(GNL_DIR)/get_next_line.c\
      

# Преобразование исходников в объектные файлы с использованием patsubst
OBJ = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))

# Библиотеки
LIBFT = $(LIBFT_DIR)/libft.a

# Основная цель
all: $(NAME)

# Сборка исполняемого файла
$(NAME): $(OBJ) $(LIBFT)
	@echo "Linking $@"
	$(CC) $(CFLAGS) $(OBJ) $(LIBFT) $(LDFLAGS) -lreadline -o $@

# Компиляция объектных файлов
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Сборка библиотеки libft
$(LIBFT):
	$(MAKE) -C $(LIBFT_DIR)

# Очистка объектных файлов
clean:
	rm -rf $(OBJ_DIR)
	$(MAKE) clean -C $(LIBFT_DIR)

# Полная очистка (включая build и исполняемый файл)
fclean: clean
	rm -rf $(BUILD_DIR)
	rm -f $(NAME)
	$(MAKE) fclean -C $(LIBFT_DIR)

# Пересборка
re: fclean all

test: re
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(NAME)

# Объявление phony целей
.PHONY: all clean fclean re
