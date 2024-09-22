/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/07 11:46:57 by vmamoten          #+#    #+#             */
/*   Updated: 2024/09/22 13:48:45 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"
#include "../libft/libft.h"

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

int	main(int argc, char **argv)

{
	t_tree	*command_node;
	char	*line;
	char	**args;

	(void)argc;
	(void)argv;
	while (1)
	{
		line = readline("minishell> ");
		if (!line)
			break ;
		args = ft_split(line, ' ');
		if (!args || !args[0])
		{
			free(line);
			if (args)
				free(args);
			continue ;
		}
		if (strcmp(args[0], "exit") == 0)
		{
			free(line);
			ft_free_args(args);
			break ;
		}
		command_node = malloc(sizeof(t_tree));
		if (!command_node)
		{
			perror("malloc");
			free(line);
			ft_free_args(args);
			return (1);
		}
		command_node->type = strdup("COMMAND");
		command_node->name = strdup(args[0]);
		command_node->left = NULL;
		command_node->right = NULL;
		ft_retranslate(command_node, args);
		free(command_node->type);
		free(command_node->name);
		free(command_node);
		ft_free_args(args);
		free(line);
	}
	return (0);
}
