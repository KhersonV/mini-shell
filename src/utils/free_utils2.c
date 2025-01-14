/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_utils2.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/12 14:10:22 by vmamoten          #+#    #+#             */
/*   Updated: 2025/01/14 19:15:17 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

void	ft_free_array(char ***array)
{
	int	i;

	if (!array || !*array)
		return;

	i = 0;
	while ((*array)[i])
	{
		free((*array)[i]);
		(*array)[i] = NULL;
		i++;
	}
	free(*array);
	*array = NULL;
}

void	free_token_list(t_token **tokens)
{
	t_token	*temp;

	if (!tokens || !*tokens)
		return;

	while (*tokens)
	{
		temp = *tokens;
		*tokens = (*tokens)->next;

		if (temp->str)
		{
			free(temp->str);
			temp->str = NULL;
		}

		free(temp);
	}

	*tokens = NULL;
}


void	free_inner_redirections(t_redirection **redir)
{
	t_redirection	*next_redir;

	if (!redir || !*redir)
		return;

	while (*redir)
	{
		next_redir = (*redir)->next;

		if ((*redir)->filename)
		{
			free((*redir)->filename);
			(*redir)->filename = NULL;
		}
		if ((*redir)->heredoc_marker)
		{
			free((*redir)->heredoc_marker);
			(*redir)->heredoc_marker = NULL;
		}

		free(*redir);
		*redir = next_redir;
	}

	*redir = NULL;
}

void	free_commands(t_exec_command **commands)
{
	t_exec_command	*current;
	t_exec_command	*next;

	if (!commands || !*commands)
		return;

	current = *commands;
	while (current)
	{
		next = current->next_cmd;

		if (current->cmd_name)
		{
			free(current->cmd_name);
			current->cmd_name = NULL;
		}
		if (current->args)
		{
			ft_free_array(&(current->args));
		}
		if (current->redirects)
		{
			free_inner_redirections(&(current->redirects));
		}

		free(current);
		current = next;
	}

	*commands = NULL;
}
