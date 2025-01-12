/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_utils2.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: snazarov <snazarov@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/12 14:10:22 by vmamoten          #+#    #+#             */
/*   Updated: 2025/01/12 14:50:16 by snazarov         ###   ########.fr       */
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

void	free_inner_redirections(t_redirection *redir)
{
	t_redirection	*next_redir;

	while (redir)
	{
		next_redir = redir->next;
		if (redir->filename)
			free(redir->filename);
		if (redir->heredoc_marker)
			free(redir->heredoc_marker);
		free(redir);
		redir = next_redir;
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
		free_inner_redirections(commands->redirects);
		commands = commands->next_cmd;
		free(temp);
	}
}
