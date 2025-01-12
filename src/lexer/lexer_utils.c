/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/12 10:29:59 by vmamoten          #+#    #+#             */
/*   Updated: 2025/01/12 10:35:16 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

int	read_single_quoted(const char *input, char *buf, int *buf_index,
		int buf_size)
{
	int	i;

	i = 1;
	while (input[i] && input[i] != '\'')
	{
		if (append_char_to_buf(buf, buf_index, buf_size, input[i]) < 0)
		{
			ft_putendl_fd("Buffer overflow in single quotes", 2);
			return (i);
		}
		i++;
	}
	if (input[i] == '\'')
	{
		i++;
	}
	return (i);
}

t_token	*create_token_node(char *name, int type)
{
	t_token	*new_node;

	new_node = (t_token *)malloc(sizeof(t_token));
	if (!new_node)
		return (NULL);
	new_node->str = ft_strdup(name);
	new_node->type = type;
	new_node->next = NULL;
	new_node->prev = NULL;
	return (new_node);
}

int	read_dollar_single(const char *input, char *buf, int *buf_index)
{
	int	i;
	int	stop;

	i = 2;
	stop = 0;
	while (input[i] && input[i] != '\'' && stop == 0)
	{
		if (append_char_to_buf(buf, buf_index, 1024, input[i]) < 0)
		{
			ft_putendl_fd("Buffer overflow in $'...'", 2);
			stop = 1;
		}
		else
			i++;
	}
	if (stop == 0 && input[i] == '\'')
		i++;
	return (i);
}

int	handle_bslash_short(const char **p, char *buf, int *buf_index)
{
	int	stop;
	int	res;

	stop = 0;
	res = append_char_to_buf(buf, buf_index, 1024, (*p)[0]);
	if (res < 0)
		stop = 1;
	*p = (*p) + 1;
	return (stop);
}

int	handle_bslash_long(const char **p, char *buf, int *buf_index)
{
	int	local_stop;
	int	res;

	local_stop = 0;
	res = append_char_to_buf(buf, buf_index, 1024, '\\');
	if (res < 0)
		local_stop = 1;
	if (local_stop == 0)
	{
		res = append_char_to_buf(buf, buf_index, 1024, (*p)[0]);
		if (res < 0)
			local_stop = 1;
	}
	*p = (*p) + 1;
	return (local_stop);
}
