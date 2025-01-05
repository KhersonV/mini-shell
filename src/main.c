/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/10 15:48:24 by vmamoten          #+#    #+#             */
/*   Updated: 2025/01/05 15:57:30 by vmamoten         ###   ########.fr       */
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


// void print_command_list(t_exec_command *cmd_list)
// {
//     t_exec_command *cmd = cmd_list;
//     int cmd_num = 1;

//     while (cmd)
//     {
//         printf("Command #%d:\n", cmd_num++);
//         printf("  Command Name: %s\n", cmd->cmd_name ? cmd->cmd_name : "(null)");

//         // Печать аргументов
//         if (cmd->args)
//         {
//             printf("  Arguments:\n");
//             for (int i = 0; cmd->args[i]; i++)
//                 printf("    [%d] %s\n", i, cmd->args[i]);
//         }
//         else
//         {
//             printf("  Arguments: None\n");
//         }

//         // Печать перенаправлений
//         if (cmd->redirects)
//         {
//             printf("  Redirections:\n");
//             t_redirection *redir = cmd->redirects;
//             while (redir)
//             {
//                 printf("    Type: %s, File: %s%s\n",
//                        (redir->type == TOKEN_REDIRECT_IN) ? "INPUT" :
//                        (redir->type == TOKEN_REDIRECT_OUT) ? "OUTPUT" :
//                        (redir->type == TOKEN_REDIRECT_APPEND) ? "APPEND" :
//                        (redir->type == TOKEN_HEREDOC) ? "HEREDOC" : "UNKNOWN",
//                        redir->filename,
//                        redir->is_heredoc ? " (Heredoc)" : "");
//                 redir = redir->next;
//             }
//         }
//         else
//         {
//             printf("  Redirections: None\n");
//         }

//         // Печать статуса выхода
//         printf("  Exit Status: %d\n\n", cmd->exit_status);

//         cmd = cmd->next_cmd;
//     }
// }

/****************************************************** FOR TEST ******************************************************************/



void print_redirections(t_redirection *redirects)
{
    while (redirects)
    {
        printf("  Redirection type: %d\n", redirects->type);
        printf("  Filename: %s\n", redirects->filename ? redirects->filename : "(null)");
        redirects = redirects->next;
    }
}

void print_exec_command(t_exec_command *commands)
{
    t_exec_command *current = commands;
    int command_index = 0;

    while (current)
    {
        printf("\nCommand %d:\n", command_index);
        printf("  Command name: %s\n", current->cmd_name ? current->cmd_name : "(null)");
        printf("  Execution path: %s\n", current->exec_path ? current->exec_path : "(null)");

        // Print arguments
        if (current->args)
        {
            printf("  Arguments:\n");
            for (int i = 0; current->args[i]; i++)
            {
                printf("    [%d]: %s\n", i, current->args[i]);
            }
        }
        else
        {
            printf("  Arguments: (null)\n");
        }

        // Print redirections
        if (current->redirects)
        {
            printf("  Redirections:\n");
            print_redirections(current->redirects);
        }
        else
        {
            printf("  Redirections: (none)\n");
        }

        printf("  Exit status: %d\n", current->exit_status);
        printf("  Next command: %s\n", current->next_cmd ? "Present" : "(null)");
        printf("  Previous command: %s\n", current->prev_cmd ? "Present" : "(null)");

        current = current->next_cmd;
        command_index++;
    }
}

void print_tokens(t_token *tokens)
{
    t_token *current = tokens;
    int token_index = 0;

    while (current)
    {
        printf("Token %d:\n", token_index);
        printf("  String: %s\n", current->str ? current->str : "(null)");
        printf("  Type: %d\n", current->type);
        printf("  Next: %s\n", current->next ? "Present" : "(null)");
        printf("  Previous: %s\n", current->prev ? "Present" : "(null)");
        current = current->next;
        token_index++;
    }
}


void print_command_list(t_exec_command *cmd_list);
void    temp_print_tokens(t_token *tokens);
t_token *tokenizer(char *user_input, t_info *info);
void restore_explicit_empty_quotes(t_token **head_ref, const char *user_input);
int validate_syntax_and_adjust(t_token **tree);


void main_initialize(t_info *info, char **envp)
{
	info->std_in_reserve = dup(STDIN_FILENO);
	info->std_out_reserve = dup(STDOUT_FILENO);
	info->curr_dir = getcwd(NULL, 0);
	info->envp = copy_envp(envp);
	info->old_dir = get_env_value(info, "OLDPWD");
	info->home = get_env_value(info, "HOME");
	info->exit_status = 0;
	init_env(info, envp);
}

void process_user_input(char *user_input, t_exec_command **command, t_info *info)
{
	t_token *tokens;
	t_exec_command *command_ptr;

	command_ptr = *command;

	tokens = tokenizer(user_input, info);
	if (!tokens)
		free(user_input);
	expansion(&tokens, info);
	adjusting_token_tree(&tokens, info);
	command_ptr = parse_tokens_to_commands(tokens);


	if (!command_ptr)
	{
		free_token_list(tokens);
		free(user_input);
	}
}

int main(int ac, char **av, char **envp)
{
	char *line;
	t_token *tokens;
	t_exec_command *commands;
	t_info info;

	// Проверка аргументов
	if (ac != 1)
	{
		printf("minishell: %s: No such file or directory\n", av[1]);
		return (1);
	}

	// Инициализация окружения и сигналов
	main_initialize(&info, envp);
	init_signals();

	// Основной цикл Shell
	while (1)
	{
		if (isatty(STDIN_FILENO)) // Если программа запущена интерактивно
			line = readline("minishell> ");
		else // Если программа запущена неинтерактивно
		{
			line = get_next_line(STDIN_FILENO);
			if (line)
			{
				char *temp = line;
				line = ft_strtrim(line, "\n");
				free(temp);
			}
		}

		if (line == NULL) // Обработка Ctrl-D
			exit_shell(&info);

		if (*line != '\0') // Добавление команды в историю
			add_history(line);

		// Лексический анализ
		// tokens = tokenize(line);
		info.input = line;
		tokens = tokenizer(line, &info);
// print_tokens(tokens);

		if (!tokens)
		{
			free(line);
			continue;
		}
		// expansion(&tokens, &info);

		adjusting_token_tree(&tokens, &info);

		        if (info.syntax_error == 1)
        {
            // У нас синтаксическая ошибка => info.exit_status уже = 2
            // Значит не запускаем parse_tokens_to_commands, execute и т.д.
            // Просто убираем токены/строку и ждём следующую команду
            free_token_list(tokens);
            free(line);
            continue; // возврат в начало цикла
        }

		// validate_syntax_and_adjust(&tokens);

		//restore_explicit_empty_quotes(&tokens, info.input);

		// printf("tokens:\n");
		// print_tokens(tokens);
		// printf("------\n");

		// Построение списка команд
		commands = parse_tokens_to_commands(tokens);

		// print_command_list(commands); // Печать команд
		// print_exec_command(commands);
		if (!commands)
		{
			free_token_list(tokens);
			free(line);
			continue;
		}

		// print_command_list(commands);
		// Выполнение команд
		execute_commands(commands, &info);

		// Очистка памяти
		free_commands(commands);
		free_token_list(tokens);
		free(line);
	}
	return (0);
}
