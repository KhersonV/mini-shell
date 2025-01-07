/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/06 19:00:23 by vmamoten          #+#    #+#             */
/*   Updated: 2025/01/07 17:31:41 by vmamoten         ###   ########.fr       */
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

void	free_redirections(t_redirection *redirects)
{
	t_redirection	*temp;

	while (redirects)
	{
		temp = redirects;
		free(redirects->filename);
		redirects = redirects->next;
		free(temp);
	}
}

void	free_array(t_exec_command *commands)
{
	t_exec_command	*current;
	t_exec_command	*next;

	current = commands;
	while (current)
	{
		next = current->next_cmd;
		free(current->cmd_name);
		free(current->exec_path);
		ft_free_array(current->args);
		free_redirections(current->redirects);
		free(current);
		current = next;
	}
}
