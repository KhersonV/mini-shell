/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_utils2.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/12 14:10:22 by vmamoten          #+#    #+#             */
/*   Updated: 2025/01/12 14:28:28 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

void	ft_free_array(char **array)
{
	int	i;

	if (!array)
		return ;
	i = 0;
	while (array[i])
	{
		free(array[i]);
		i++;
	}
	free(array);
}

void	free_token_list(t_token *tokens)
{
	t_token	*temp;

	while (tokens)
	{
		temp = tokens;
		free(temp->str);
		tokens = tokens->next;
		free(temp);
	}
}

void	free_commands(t_exec_command *commands)
{
	t_exec_command	*temp;

	while (commands)
	{
		temp = commands;
		free(commands->cmd_name);
		ft_free_array(commands->args);
		free_redirections(commands->redirects);
		commands = commands->next_cmd;
		free(temp);
	}
}
