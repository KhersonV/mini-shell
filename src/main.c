/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: admin <admin@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/07 11:46:57 by vmamoten          #+#    #+#             */
/*   Updated: 2024/10/13 15:29:16 by admin            ###   ########.fr       */
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
int	main(int argc, char **argv, char **envp)
{
	t_info		*info;
	t_command	*cmd;
	char		*line;
	char		**args;

	(void)argc;
	(void)argv;
	info = malloc(sizeof(t_command));
	if (!info)
	{
		perror("malloc");
		return (1);
	}
	info->envp = copy_envp(envp);
	if (!info->envp)
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
		if (!args || !args[0])
		{
			ft_free_args(args);
			free(line);
			continue ;
		}
		if (ft_strcmp(args[0], "exit") == 0)
		{
			ft_free_args(args);
			free(line);
			break ;
		}
		cmd = malloc(sizeof(t_command));
		if (!cmd)
		{
			perror("malloc");
			ft_free_args(args);
			free(line);
			free(info);
			return (1);
		}
		cmd->name = ft_strdup(args[0]);
		cmd->args = args;
		cmd->file = "file.txt";
		cmd->redirection = ">>";
		cmd->next = NULL;
		info->pipes = 4;
		
		ft_retranslate(cmd, info, info->envp);
		free(cmd->name);
		free(cmd);
		
		free(line);
	}
	ft_free_args(info->envp);
	free(info);
	return (0);
}
