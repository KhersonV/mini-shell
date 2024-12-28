/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lynchsama <lynchsama@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/10 15:48:24 by vmamoten          #+#    #+#             */
/*   Updated: 2024/12/28 21:57:10 by lynchsama        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"






/****************************************************** FOR TEST ******************************************************************/




// /**
//  * Utility function to create and zero-initialize a new command struct.
//  */
// static t_exec_command *create_new_command(void)
// {
//     t_exec_command *cmd = (t_exec_command *)calloc(1, sizeof(t_exec_command));
//     if (!cmd)
//         return NULL;
//     // Initialize pipe_fds to something (e.g., -1) to indicate uninitialized
//     return cmd;
// }

// /**
//  * Utility function to create a new redirection struct.
//  */
// // static t_redirection *create_new_redir(const char *filename,
// //                                        int fd,
// //                                        int is_append,
// //                                        int is_heredoc,
// //                                        const char *heredoc_marker,
// //                                        int type)
// // {
// //     t_redirection *redir = (t_redirection *)calloc(1, sizeof(t_redirection));
// //     if (!redir)
// //         return NULL;

// //     if (filename)
// //         redir->filename = strdup(filename);

// //     redir->fd = fd;
// //     redir->is_append = is_append;
// //     redir->is_heredoc = is_heredoc;

// //     if (heredoc_marker)
// //         redir->heredoc_marker = strdup(heredoc_marker);

// //     redir->type = type;

// //     return redir;
// // }

// /**
//  * Build a fake linked list of commands for testing.
//  * Returns the head of the list (Command #1).
//  *
//  * This example pipeline is:
//  *   (1) echo "Hello from command1"
//  *    |
//  *   (2) grep Hello < my_input_file >> my_output_file
//  *    |
//  *   (3) wc -l << EOF
//  */t_exec_command *build_fake_command_list(void)
// {
//     /*
//      * --------------------------------
//      * Command #1: cat
//      * --------------------------------
//      */
//     t_exec_command *cmd1 = create_new_command();
//     cmd1->cmd_name = strdup("cat");
//     cmd1->exec_path = NULL;
//     // Build args
//     char **args1 = (char **)calloc(2, sizeof(char *));
//     args1[0] = strdup("cat");
//     args1[1] = NULL;
//     cmd1->args = args1;
//     cmd1->redirects = NULL;
//     cmd1->exit_status = 0;

//     /*
//      * --------------------------------
//      * Command #2: cat
//      * --------------------------------
//      */
//     t_exec_command *cmd2 = create_new_command();
//     cmd2->cmd_name = strdup("cat");
//     cmd2->exec_path = NULL;
//     // Build args
//     char **args2 = (char **)calloc(2, sizeof(char *));
//     args2[0] = strdup("cat");
//     args2[1] = NULL;
//     cmd2->args = args2;
//     cmd2->redirects = NULL;
//     cmd2->exit_status = 0;

//     /*
//      * --------------------------------
//      * Command #3: ls
//      * --------------------------------
//      */
//     t_exec_command *cmd3 = create_new_command();
//     cmd3->cmd_name = strdup("ls");
//     cmd3->exec_path = NULL;
//     // Build args
//     char **args3 = (char **)calloc(2, sizeof(char *));
//     args3[0] = strdup("ls");
//     args3[1] = NULL;
//     cmd3->args = args3;
//     cmd3->redirects = NULL;
//     cmd3->exit_status = 0;

//     /*
//      * Link the commands together: cmd1 -> cmd2 -> cmd3
//      */
//     cmd1->next_cmd = cmd2;
//     cmd2->prev_cmd = cmd1;

//     cmd2->next_cmd = cmd3;
//     cmd3->prev_cmd = cmd2;

//     // Return the head of this chain
//     return cmd1;
// }





// int main(int ac, char **av, char **envp)
// {
//     t_exec_command  *commands;
//     t_info          info;

//     // Проверка аргументов
//     if (ac != 1)
//     {
//         printf("minishell: %s: No such file or directory\n", av[1]);
//         return (1);
//     }

//     // Инициализация окружения и сигналов
//     init_env(&info, envp);
//     init_signals();

//     // Тестирование фиктивного списка команд cat | cat | ls
//     commands = build_fake_command_list();
//     if (!commands)
//     {
//         printf("Failed to build fake command list\n");
//         return (1);
//     }

//     // Выполнение команд
//     execute_commands(commands, &info);

//     // Очистка памяти
//     free_commands(commands);

//     return (0);
// }



/****************************************************** FOR TEST ******************************************************************/


int	main(int ac, char **av, char **envp)
{
	char			*line;
	t_token			*tokens;
	t_exec_command	*commands;
	t_info			info;

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

		// Построение списка команд
		commands = parse_tokens_to_commands(tokens);

		if (!commands)
		{
			free_token_list(tokens);
			free(line);
			continue;
		}

		// Выполнение команд
		execute_commands(commands, &info);

		// Очистка памяти
		free_commands(commands);
		free_token_list(tokens);
		free(line);
	}
	return (0);
}
