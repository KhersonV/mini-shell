/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/10 15:48:24 by vmamoten          #+#    #+#             */
/*   Updated: 2024/12/19 11:55:09 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"


int	main(int ac, char **av, char **envp)
{
	char			*line;
	t_token			*tokens;
	t_exec_command	*commands;
	t_info			info;
	t_node			*ast;

	// Проверка аргументов
	if (ac != 1)
	{
		printf("minishell: %s: No such file or directory\n", av[1]);
		return (1);
	}

	// Инициализация окружения и сигналов
	init_env(&info, envp);
	init_signals();

	// Основной цикл Shell
	while (1)
	{
		line = readline("minishell> ");
		if (line == NULL) // Обработка Ctrl-D
			exit_shell(&info);
		if (*line != '\0') // Добавление команды в историю
			add_history(line);

		// Лексический анализ
		tokens = tokenize(line);
		if (!tokens)
		{
			free(line);
			continue;
		}

		// Удаление пробелов и настройка дерева токенов
		remove_spaces(&tokens);
		adjusting_token_tree(&tokens);

		// Построение AST
		ast = parse_tokens(tokens);
		if (!ast)
		{
			free_token_list(tokens);
			free(line);
			continue;
		}

		// Выполнение команд
		commands = ast_to_exec_commands(ast); // Преобразование AST в команды
		if (commands)
		{
			execute_commands(commands, &info);
			free_commands(commands);
		}

		// Очистка памяти
		free_token_list(tokens);
		free_ast(ast);
		free(line);
	}
	return (0);
}
