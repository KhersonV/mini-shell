/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils8.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/12 10:40:06 by vmamoten          #+#    #+#             */
/*   Updated: 2025/01/12 10:40:27 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

void	skip_spaces(t_lexer_params *params)
{
	while (is_space_char(params->input[*params->i]))
	{
		flush_buf_if_needed(params->head, params->buf, params->buf_index);
		(*params->i)++;
	}
}

int	handle_double_quote(t_lexer_params *params, t_info *info)
{
	int		old_index;
	int		consumed;
	char	c;

	if (params->input[*params->i] != '"')
		return (0);
	old_index = *params->buf_index;
	consumed = read_double_quoted(&params->input[*params->i], params, info);
	*params->i = *params->i + consumed;
	if (*params->buf_index == old_index)
	{
		c = params->input[*params->i];
		if (c == '\0' || is_space_char(c) || is_operator_char(c))
		{
			params->buf[*params->buf_index] = '\0';
			*params->buf_index = *params->buf_index + 1;
			flush_buf_if_needed(params->head, params->buf, params->buf_index);
		}
	}
	return (1);
}

int	handle_operator_char(t_lexer_params *params, int *i)
{
	if (!is_operator_char(params->input[*i]))
		return (0);
	flush_buf_if_needed(params->head, params->buf, params->buf_index);
	*params->head = add_operator_token(*params->head, params->input[*i],
			params->input[*i + 1], i);
	*i = *i + 1;
	return (1);
}

int	handle_single_quote(t_lexer_params *params)
{
	int		old_index;
	int		consumed;
	char	c;

	if (params->input[*params->i] != '\'')
		return (0);
	old_index = *params->buf_index;
	consumed = read_single_quoted(&params->input[*params->i], params->buf,
			params->buf_index, params->buf_size);
	*params->i = *params->i + consumed;
	if (*params->buf_index == old_index)
	{
		c = params->input[*params->i];
		if (c == '\0' || is_space_char(c) || is_operator_char(c))
		{
			params->buf[*params->buf_index] = '\0';
			*params->buf_index = *params->buf_index + 1;
			flush_buf_if_needed(params->head, params->buf, params->buf_index);
		}
	}
	return (1);
}

int	handle_unquoted(t_lexer_params *params, t_info *info)
{
	int	consumed;

	if (is_space_char(params->input[*params->i])
		|| is_operator_char(params->input[*params->i])
		|| params->input[*params->i] == '\''
		|| params->input[*params->i] == '"')
	{
		return (0);
	}
	consumed = read_unquoted(&params->input[*params->i], params, info);
	*params->i = *params->i + consumed;
	return (1);
}
