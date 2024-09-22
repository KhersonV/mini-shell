/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/07 11:46:57 by vmamoten          #+#    #+#             */
/*   Updated: 2024/09/22 12:25:34 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	main(int argc, char **argv)
{
	t_tree	*command_node;
	char	*args[] = {"echo", "Hello, World!", "123", NULL};

	(void)argc;
	(void)argv;
	command_node = malloc(sizeof(t_tree));
	if (!command_node)
	{
		perror("malloc");
		return (1);
	}
	command_node->type = ft_strdup("COMMAND");
	command_node->name = ft_strdup("echo");
	command_node->left = NULL;
	command_node->right = NULL;
	ft_retranslate(command_node, args);
	free(command_node->type);
	free(command_node->name);
	free(command_node);
	return (0);
}
