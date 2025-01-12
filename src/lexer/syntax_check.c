/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   syntax_check.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/12 10:31:20 by vmamoten          #+#    #+#             */
/*   Updated: 2025/01/12 10:37:05 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

static int	check_initial_syntax_errors(t_token **tree, t_info *info)
{
	if (!tree || !*tree)
		return (0);
	if ((*tree)->type == TOKEN_PIPE)
	{
		ft_putstr_fd("minishell: syntax error near unexpected token `|'\n", 2);
		info->syntax_error = 1;
		info->exit_status = 2;
		return (-1);
	}
	return (0);
}

static void	process_token_list(t_token *tree, t_info *info)
{
	t_token	*curr;
	int		command_found;

	curr = tree;
	command_found = 0;
	while (curr)
	{
		if (curr->type == TOKEN_PIPE)
		{
			command_found = 0;
			if (check_pipes_error(curr, info) == -1)
				return ;
		}
		adjust_word_token(curr, &command_found);
		if (redirect_check(curr, info) == -1)
			return ;
		curr = curr->next;
	}
}

void	adjusting_token_tree(t_token **tree, t_info *info)
{
	if (check_initial_syntax_errors(tree, info) == -1)
		return ;
	process_token_list(*tree, info);
}
