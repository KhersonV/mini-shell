/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils6.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/12 10:41:28 by vmamoten          #+#    #+#             */
/*   Updated: 2025/01/12 10:41:43 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

void	append_expanded_dquotes(const char *input, int *i,
		t_lexer_params *params, t_info *info)
{
	int		var_consumed;
	char	*expanded;
	int		k;
	int		len;

	var_consumed = 0;
	expanded = expand_dollar(&input[*i], &var_consumed, info);
	if (!expanded)
		return ;
	k = 0;
	len = ft_strlen(expanded);
	while (k < len)
	{
		if (append_char_to_buf(params->buf, params->buf_index, params->buf_size,
				expanded[k]) < 0)
		{
			free(expanded);
			return ;
		}
		k++;
	}
	free(expanded);
	*i = *i + var_consumed;
}

int	handle_dollar_in_dquotes(const char *input, int *i, t_lexer_params *params,
		t_info *info)
{
	append_expanded_dquotes(input, i, params, info);
	return (0);
}

int	handle_escape_sequence(const char *input, int *i, t_lexer_params *params)
{
	(*i)++;
	if (!input[*i])
		return (-1);
	if (input[*i] == '"' || input[*i] == '$' || input[*i] == '\\')
	{
		if (append_char_to_buf(params->buf, params->buf_index, params->buf_size,
				input[*i]) < 0)
			return (-1);
		(*i)++;
	}
	else
	{
		if (append_char_to_buf(params->buf, params->buf_index, params->buf_size,
				'\\') < 0)
			return (-1);
		if (append_char_to_buf(params->buf, params->buf_index, params->buf_size,
				input[*i]) < 0)
			return (-1);
		(*i)++;
	}
	return (0);
}

int	read_double_quoted(const char *input, t_lexer_params *params, t_info *info)
{
	int	i;

	i = 1;
	while (input[i] && input[i] != '"')
	{
		if (input[i] == '\\')
		{
			if (handle_escape_sequence(input, &i, params) < 0)
				return (i);
		}
		else if (input[i] == '$')
		{
			handle_dollar_in_dquotes(input, &i, params, info);
		}
		else
		{
			if (append_char_to_buf(params->buf, params->buf_index,
					params->buf_size, input[i]) < 0)
				return (i);
			i++;
		}
	}
	if (input[i] == '"')
		i++;
	return (i);
}

int	handle_dollar(const char *input, int *i, t_lexer_params *params,
		t_info *info)
{
	append_expanded_unquoted(input, i, params, info);
	return (0);
}
