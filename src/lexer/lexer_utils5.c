/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils5.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/12 10:35:33 by vmamoten          #+#    #+#             */
/*   Updated: 2025/01/12 10:35:49 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

int	handle_dquotes_backslash(const char **p, char *buf, int *buf_index)
{
	int	stop;
	int	check;

	stop = 0;
	*p = (*p) + 1;
	if ((*p)[0] == '\0')
		return (stop);
	if (ft_strchr("\"$\\", (*p)[0]))
	{
		check = handle_bslash_short(p, buf, buf_index);
		if (check != 0)
			stop = 1;
	}
	else
	{
		check = handle_bslash_long(p, buf, buf_index);
		if (check != 0)
			stop = 1;
	}
	return (stop);
}

int	handle_dquotes_dollar(const char **p, char *buf, int *buf_index,
		t_info *info)
{
	int		stop;
	int		var_consumed;
	char	*expanded;
	int		copy_stop;

	stop = 0;
	var_consumed = 0;
	expanded = expand_dollar(*p, &var_consumed, info);
	if (!expanded)
		stop = 1;
	else
	{
		copy_stop = 0;
		while (expanded[copy_stop] != '\0' && stop == 0)
		{
			if (append_char_to_buf(buf, buf_index, 1024,
					expanded[copy_stop]) < 0)
				stop = 1;
			else
				copy_stop = copy_stop + 1;
		}
		free(expanded);
		*p = (*p) + var_consumed;
	}
	return (stop);
}

int	handle_dquotes_normal_char(const char **p, char *buf, int *buf_index)
{
	int	stop;
	int	res;

	stop = 0;
	res = append_char_to_buf(buf, buf_index, 1024, (*p)[0]);
	if (res < 0)
		stop = 1;
	else
		*p = (*p) + 1;
	return (stop);
}

int	read_dollar_double(const char *input, char *buf, int *buf_index,
		t_info *info)
{
	const char	*p;
	const char	*start;
	int			stop;

	start = &input[2];
	p = start;
	stop = 0;
	while (p[0] && p[0] != '"' && stop == 0)
	{
		if (p[0] == '\\')
			stop = handle_dquotes_backslash(&p, buf, buf_index);
		else if (p[0] == '$')
			stop = handle_dquotes_dollar(&p, buf, buf_index, info);
		else
			stop = handle_dquotes_normal_char(&p, buf, buf_index);
	}
	if (stop == 0 && p[0] == '"')
		p++;
	return (p - input);
}

char	*read_dollar_quoted(const char *input, int *consumed, t_info *info)
{
	char	quote;
	char	buf[1024];
	int		buf_index;
	int		i;

	quote = input[1];
	buf_index = 0;
	i = 0;
	if (quote == '\'')
		i = read_dollar_single(input, buf, &buf_index);
	else if (quote == '"')
		i = read_dollar_double(input, buf, &buf_index, info);
	buf[buf_index] = '\0';
	*consumed = i;
	return (ft_strdup(buf));
}
