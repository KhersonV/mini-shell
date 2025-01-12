/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils7.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/12 10:40:36 by vmamoten          #+#    #+#             */
/*   Updated: 2025/01/12 10:40:59 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

int	handle_escape(const char *input, int *i, t_lexer_params *params)
{
	(*i)++;
	if (!input[*i])
		return (-1);
	if (ft_strchr("$\\\"'", input[*i]))
	{
		if (append_char_to_buf(params->buf, params->buf_index, params->buf_size,
				input[*i]) < 0)
			return (-1);
	}
	else
	{
		if (append_char_to_buf(params->buf, params->buf_index, params->buf_size,
				'\'') < 0)
			return (-1);
		if (append_char_to_buf(params->buf, params->buf_index, params->buf_size,
				input[*i]) < 0)
			return (-1);
	}
	(*i)++;
	return (0);
}

int	handle_default(const char *input, int *i, t_lexer_params *params)
{
	if (append_char_to_buf(params->buf, params->buf_index, params->buf_size,
			input[*i]) < 0)
		return (-1);
	(*i)++;
	return (0);
}

int	should_break(char c)
{
	return (is_space_char(c) || is_operator_char(c) || c == '\'' || c == '"');
}

int	read_unquoted(const char *input, t_lexer_params *params, t_info *info)
{
	int	i;

	i = 0;
	while (input[i] != '\0')
	{
		if (should_break(input[i]))
			break ;
		if (input[i] == '$')
		{
			if (handle_dollar(input, &i, params, info) < 0)
				return (i);
			continue ;
		}
		if (input[i] == '\\')
		{
			if (handle_escape(input, &i, params) < 0)
				return (i);
			continue ;
		}
		if (handle_default(input, &i, params) < 0)
			return (i);
	}
	return (i);
}

int	process_token(t_lexer_params params, t_info *info)
{
	if (handle_operator_char(&params, params.i))
		return (1);
	if (handle_single_quote(&params))
		return (1);
	if (handle_double_quote(&params, info))
		return (1);
	if (handle_unquoted(&params, info))
		return (1);
	return (0);
}
