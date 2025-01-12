/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expansion.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/12 10:27:56 by vmamoten          #+#    #+#             */
/*   Updated: 2025/01/12 10:32:11 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

static int	is_valid_var_char(char c)
{
	return (ft_isalnum((unsigned char)c) || c == '_');
}

static char	*read_special_var(const char *input, int *consumed)
{
	if (input[1] == '?')
	{
		*consumed = 2;
		return (ft_strdup("?"));
	}
	return (NULL);
}

static char	*read_variable_name(const char *input, int *consumed)
{
	char	var_buf[256];
	int		var_idx;
	int		i;

	var_idx = 0;
	i = 1;
	while (input[i] && is_valid_var_char(input[i]))
	{
		if (var_idx < 255)
		{
			var_buf[var_idx++] = input[i];
		}
		i++;
	}
	var_buf[var_idx] = '\0';
	*consumed = i;
	if (var_idx == 0)
	{
		return (ft_strdup("$"));
	}
	return (ft_strdup(var_buf));
}

char	*read_var_name(const char *input, int *consumed)
{
	char	*special_var;

	if (input[0] != '$')
	{
		*consumed = 0;
		return (NULL);
	}
	special_var = read_special_var(input, consumed);
	if (special_var)
	{
		return (special_var);
	}
	return (read_variable_name(input, consumed));
}

t_token	*add_operator_token(t_token *curr, char current_char, char next_char,
		int *i)
{
	if (current_char == '|')
		curr = add_token(curr, "|", TOKEN_PIPE);
	else if (current_char == '<')
	{
		if (next_char == '<')
		{
			curr = add_token(curr, "<<", TOKEN_HEREDOC);
			(*i)++;
		}
		else
			curr = add_token(curr, "<", TOKEN_REDIRECT_IN);
	}
	else if (current_char == '>')
	{
		if (next_char == '>')
		{
			curr = add_token(curr, ">>", TOKEN_REDIRECT_APPEND);
			(*i)++;
		}
		else
			curr = add_token(curr, ">", TOKEN_REDIRECT_OUT);
	}
	return (curr);
}
