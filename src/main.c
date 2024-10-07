/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: admin <admin@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/07 11:46:57 by vmamoten          #+#    #+#             */
/*   Updated: 2024/10/07 16:26:59 by admin            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

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
int main(int argc, char **argv, char **envp)
{
    t_command   *cmd;
    char        *line;
    char        **args;
    char        **shell_env;

    (void)argc;
    (void)argv;
    shell_env = copy_envp(envp);
    if (!shell_env)
    {
        perror("Failed to copy environment");
        return (1);
    }
    while (1)
    {
        line = readline("minishell> ");
        if (!line)
            break ;
        args = ft_split(line, ' ');
        free(line);
        if (!args || !args[0])
        {
            ft_free_args(args);
            continue ;
        }
        if (ft_strcmp(args[0], "exit") == 0)
        {
            ft_free_args(args);
            break ;
        }
        cmd = malloc(sizeof(t_command));
        if (!cmd)
        {
            perror("malloc");
            ft_free_args(args);
            return (1);
        }
        cmd->name = ft_strdup(args[0]);
        cmd->args = args;
        cmd->infile = NULL;
        cmd->outfile = NULL;
        cmd->append = 0;
        cmd->next = NULL;
        ft_retranslate(cmd, shell_env);
        free(cmd->name);
        free(cmd);
    }
    ft_free_args(shell_env);
    return (0);
}
