
#include "../../include/minishell.h"

void	ft_free_args(char **args)
{
	int	i;

	i = 0;
	if (!args)
		return ;
	while (args[i])
	{
		free(args[i]);
		i++;
	}
	free(args);
}

void	ft_free_array(char **array)
{
	int	i;

	i = 0;
	if (!array)
		return ;
	while (array[i])
	{
		free(array[i]);
		i++;
	}
	free(array);
}

void free_token_list(t_token *tokens)
{
    t_token *temp;

    while (tokens)
    {
        temp = tokens;
        free(temp->str);
        tokens = tokens->next;
        free(temp);
    }
}


void free_commands(t_exec_command *commands)
{
    t_exec_command *temp;

    while (commands)
    {
        temp = commands;
        free(commands->cmd_name);
        ft_free_array(commands->args); // Освобождение массива аргументов
        free_redirections(commands->redirects); // Освобождение редиректов
        commands = commands->next_cmd;
        free(temp);
    }
}



void free_redirections(t_redirection *redirects)
{
    t_redirection *temp;

    while (redirects)
    {
        temp = redirects;
        free(redirects->filename); // Освобождаем имя файла
        redirects = redirects->next;
        free(temp); // Освобождаем текущую структуру
    }
}

void exit_shell(t_info *info)
{
    free_env(info);
    // printf("Exiting minishell...\n");
    exit(info->exit_status);
}
