/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/10 15:48:24 by vmamoten          #+#    #+#             */
/*   Updated: 2024/12/30 11:53:11 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"


/****************************************************** FOR TEST ******************************************************************/


// #include "../include/minishell.h"

// // Declare the utility function
// static t_exec_command *create_new_command(void);

// static t_exec_command *create_new_command(void)
// {
//     t_exec_command *cmd = (t_exec_command *)calloc(1, sizeof(t_exec_command));
//     if (!cmd)
//         return NULL;
//     return cmd;
// }

// /**
//  * Build a fake linked list of commands for testing.
//  * This pipeline is:
//  *   env VAR=HELLOOO echo $VAR
//  */
// t_exec_command *build_fake_command_list(void)
// {
//     // Command #1: env VAR=HELLOOO
//     t_exec_command *cmd1 = create_new_command();
//     cmd1->cmd_name = strdup("env");
//     cmd1->exec_path = NULL;

//     char **args1 = (char **)calloc(3, sizeof(char *));
//     args1[0] = strdup("env");
//     args1[1] = strdup("VAR=HELLOOO");
//     args1[2] = NULL;
//     cmd1->args = args1;
//     cmd1->redirects = NULL;
//     cmd1->exit_status = 0;

//     // Command #2: echo $VAR
//     t_exec_command *cmd2 = create_new_command();
//     cmd2->cmd_name = strdup("echo");
//     cmd2->exec_path = NULL;

//     char **args2 = (char **)calloc(3, sizeof(char *));
//     args2[0] = strdup("echo");
//     args2[1] = strdup("$VAR"); // This relies on shell-like expansion logic
//     args2[2] = NULL;
//     cmd2->args = args2;
//     cmd2->redirects = NULL;
//     cmd2->exit_status = 0;

//     // Link the commands together
//     cmd1->next_cmd = cmd2;
//     cmd2->prev_cmd = cmd1;

//     return cmd1;
// }

// int main(int ac, char **av, char **envp)
// {
//     t_exec_command *commands;
//     t_info info;

//     if (ac != 1)
//     {
//         printf("minishell: %s: No such file or directory\n", av[1]);
//         return 1;
//     }

//     // Initialize the shell environment and signals
//     init_env(&info, envp);
//     init_signals();

//     // Build the fake command pipeline
//     commands = build_fake_command_list();
//     if (!commands)
//     {
//         printf("Failed to build fake command list\n");
//         return 1;
//     }

//     // Execute the commands
//     execute_commands(commands, &info);

//     // Free allocated resources
//     free_commands(commands);

//     return 0;
// }


/****************************************************** FOR TEST ******************************************************************/


void main_initialize(t_info *info, char **envp)
{
	info->std_in_reserve = dup(STDIN_FILENO);
	info->std_out_reserve = dup(STDOUT_FILENO);
	info->old_dir = NULL;
	info->curr_dir = getcwd(NULL, 0);
	info->envp = copy_envp(envp);
	info->old_dir = get_env_value(info, "OLDPWD");
	info->exit_status = 0;
}

void process_user_input(char *user_input, t_exec_command **command, t_info *info)
{
	t_token *tokens;
	t_exec_command *command_ptr;

	command_ptr = *command;

	tokens = tokenize(user_input);
	if (!tokens)
		free(user_input);
	expansion(&tokens, info);
	adjusting_token_tree(&tokens);
	command_ptr = parse_tokens_to_commands(tokens);

	if (!command_ptr)
	{
		free_token_list(tokens);
		free(user_input);
	}
}


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
		expansion(&tokens, &info);

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
