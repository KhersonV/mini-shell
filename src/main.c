/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/10 15:48:24 by vmamoten          #+#    #+#             */
/*   Updated: 2025/01/06 15:48:06 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

 t_exec_command *g_commands = NULL;



void free_array(t_exec_command *commands)
{
    t_exec_command *current;
    t_exec_command *next;

    current = commands;
    while (current)
    {
        next = current->next_cmd;

        // Освобождаем память для имени команды
        if (current->cmd_name)
            free(current->cmd_name);

        // Освобождаем путь к исполняемому файлу
        if (current->exec_path)
            free(current->exec_path);

        // Освобождаем аргументы
        if (current->args)
        {
            char **args = current->args;
            while (*args)
            {
                free(*args);
                args++;
            }
            free(current->args);
        }

        // Освобождаем перенаправления
        if (current->redirects)
        {
            t_redirection *redir = current->redirects;
            t_redirection *next_redir;
            while (redir)
            {
                next_redir = redir->next;
                if (redir->filename)
                    free(redir->filename);
                free(redir);
                redir = next_redir;
            }
        }

        // Освобождаем текущую команду
        free(current);
        current = next;
    }
}



// void print_redirections(t_redirection *redirects)
// {
//     while (redirects)
//     {
//         printf("  Redirection type: %d\n", redirects->type);
//         printf("  Filename: %s\n", redirects->filename ? redirects->filename : "(null)");
//         redirects = redirects->next;
//     }
// }

// void print_exec_command(t_exec_command *commands)
// {
//     t_exec_command *current = commands;
//     int command_index = 0;

//     while (current)
//     {
//         printf("\nCommand %d:\n", command_index);
//         printf("  Command name: %s\n", current->cmd_name ? current->cmd_name : "(null)");
//         printf("  Execution path: %s\n", current->exec_path ? current->exec_path : "(null)");

//         // Print arguments
//         if (current->args)
//         {
//             printf("  Arguments:\n");
//             for (int i = 0; current->args[i]; i++)
//             {
//                 printf("    [%d]: %s\n", i, current->args[i]);
//             }
//         }
//         else
//         {
//             printf("  Arguments: (null)\n");
//         }

//         // Print redirections
//         if (current->redirects)
//         {
//             printf("  Redirections:\n");
//             print_redirections(current->redirects);
//         }
//         else
//         {
//             printf("  Redirections: (none)\n");
//         }

//         printf("  Exit status: %d\n", current->exit_status);
//         printf("  Next command: %s\n", current->next_cmd ? "Present" : "(null)");
//         printf("  Previous command: %s\n", current->prev_cmd ? "Present" : "(null)");

//         current = current->next_cmd;
//         command_index++;
//     }
// }

// void print_tokens(t_token *tokens)
// {
//     t_token *current = tokens;
//     int token_index = 0;

//     while (current)
//     {
//         printf("Token %d:\n", token_index);
//         printf("  String: %s\n", current->str ? current->str : "(null)");
//         printf("  Type: %d\n", current->type);
//         printf("  Next: %s\n", current->next ? "Present" : "(null)");
//         printf("  Previous: %s\n", current->prev ? "Present" : "(null)");
//         current = current->next;
//         token_index++;
//     }
// }

// void print_command_list(t_exec_command *cmd_list);
// t_token *tokenizer(char *user_input, t_info *info);

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
	// adjusting_token_tree(&tokens, info);
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

	if (ac != 1)
	{
		printf("minishell: %s: No such file or directory\n", av[1]);
		return (1);
	}
	main_initialize(&info, envp);
	init_signals();
	while (1)
	{
		if (isatty(STDIN_FILENO))
			line = readline("minishell> ");
		else
		{
			line = get_next_line(STDIN_FILENO);
			if (line)
			{
				char *temp = line;
				line = ft_strtrim(line, "\n");
				free(temp);
			}
		}
		if (line == NULL)
			exit_shell(&info);
		if (*line != '\0')
			add_history(line);
		info.input = line;
		tokens = tokenizer(line, &info);
		if (!tokens)
		{
			free(line);
			continue;
		}
		adjusting_token_tree(&tokens, &info);
		        if (info.syntax_error == 1)
        {
            free_token_list(tokens);
            free(line);
            continue; 
        }
		commands = parse_tokens_to_commands(tokens);
		if (!commands)
		{
			free_token_list(tokens);
			free(line);
			continue;
		}
		g_commands = commands;
		execute_commands(commands, &info);
		g_commands = NULL;
		free_array(commands);
		free_token_list(tokens);
		free(line);
	}
	return (0);
}

