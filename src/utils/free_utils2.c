/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_utils2.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/12 14:10:22 by vmamoten          #+#    #+#             */
/*   Updated: 2025/01/15 15:22:34 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

void	ft_free_array(char ***array)
{
	int	i;

	if (!array || !*array)
		return ;
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
		return ;
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

void	free_redirections(t_redirection **redir)
{
	t_redirection	*next_redir;

	if (!redir || !*redir)
		return ;
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
