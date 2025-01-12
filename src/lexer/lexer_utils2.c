/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils2.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/12 10:29:18 by vmamoten          #+#    #+#             */
/*   Updated: 2025/01/12 10:33:59 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

char	*handle_variable_expansion(const char *input, int *consumed,
		t_info *info)
{
	int		var_consumed;
	char	*var_name;
	char	*expanded;

	var_consumed = 0;
	var_name = read_var_name(input, &var_consumed);
	if (!var_name)
	{
		*consumed = 0;
		return (ft_strdup(""));
	}
	*consumed = var_consumed;
	expanded = handle_special_variable(var_name, info);
	if (expanded)
	{
		free(var_name);
		return (expanded);
	}
	expanded = ft_expand_variable(var_name, info);
	free(var_name);
	return (expanded);
}

char	*expand_dollar(const char *input, int *consumed, t_info *info)
{
	char	*result;

	if (input[1] == '\'' || input[1] == '"')
	{
		result = handle_quoted_dollar(input, consumed);
		if (result)
			return (result);
		return (read_dollar_quoted(input, consumed, info));
	}
	return (handle_variable_expansion(input, consumed, info));
}

t_token	*add_token(t_token *node, char *name, int type)
{
	t_token	*new_node;
	t_token	*curr;

	new_node = create_token_node(name, type);
	if (!new_node)
		return (NULL);
	if (!node)
		return (new_node);
	curr = node;
	while (curr->next)
	{
		curr = curr->next;
	}
	curr->next = new_node;
	new_node->prev = curr;
	return (node);
}

void	flush_buf_if_needed(t_token **curr, char *buf, int *buf_index)
{
	if (*buf_index > 0)
	{
		buf[*buf_index] = '\0';
		*curr = add_token(*curr, buf, TOKEN_WORD);
		*buf_index = 0;
	}
}

int	is_quotes_closed(const char *start)
{
	char	quote;
	int		i;

	quote = *start;
	i = 1;
	while (start[i])
	{
		if (start[i] == quote)
			return (1);
		i++;
	}
	return (0);
}
