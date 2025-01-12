/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils4.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/12 10:34:27 by vmamoten          #+#    #+#             */
/*   Updated: 2025/01/12 10:49:00 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

int	is_early_termination(char next_char, char quote)
{
	return (next_char == '\0' || is_space_char(next_char)
		|| is_operator_char(next_char) || next_char == quote);
}

int	find_closing_quote(const char *input, char quote)
{
	int	j;

	j = 2;
	while (input[j])
	{
		if (input[j] == quote)
			return (j);
		j++;
	}
	return (-1);
}

char	*handle_no_closing_quote(int *consumed)
{
	*consumed = 1;
	return (ft_strdup("$"));
}

char	*handle_special_variable(const char *var_name, t_info *info)
{
	if (ft_strcmp(var_name, "?") == 0)
	{
		return (ft_itoa(info->exit_status));
	}
	if (ft_strcmp(var_name, "$") == 0)
	{
		return (ft_strdup("$"));
	}
	return (NULL);
}

char	*handle_quoted_dollar(const char *input, int *consumed)
{
	char	quote;
	char	next_char;
	int		closing_quote_index;

	quote = input[1];
	next_char = input[2];
	if (is_early_termination(next_char, quote))
		return (handle_no_closing_quote(consumed));
	closing_quote_index = find_closing_quote(input, quote);
	if (closing_quote_index == -1)
		return (handle_no_closing_quote(consumed));
	return (NULL);
}
