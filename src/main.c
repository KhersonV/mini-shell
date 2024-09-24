/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: admin <admin@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/07 11:46:57 by vmamoten          #+#    #+#             */
/*   Updated: 2024/09/24 20:50:46 by admin            ###   ########.fr       */
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
	t_command *command_node;
	char *line;
	char **args;

	(void)argc;
	(void)argv;
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
		if (strcmp(args[0], "exit") == 0)
		{
			ft_free_args(args);
			break ;
		}
		command_node = malloc(sizeof(t_command));
		if (!command_node)
		{
			perror("malloc");
			ft_free_args(args);
			return (1);
		}
		command_node->name = ft_strdup(args[0]);
		command_node->args = args;
        command_node->infile = NULL;
        command_node->outfile = NULL;
        command_node->append = 0;
        command_node->next = NULL;
		ft_retranslate(command_node, envp);
		free(command_node->name);
		free(command_node);
		ft_free_args(args);
	}
	return (0);
}
