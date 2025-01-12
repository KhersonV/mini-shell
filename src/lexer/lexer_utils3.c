/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils3.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: snazarov <snazarov@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/12 10:29:33 by vmamoten          #+#    #+#             */
/*   Updated: 2025/01/12 16:13:00 by snazarov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

char	*ft_expand_variable(char *var_name, t_info *info)
{
	char	*val;

	val = get_env_value(info, var_name);
	if (val == NULL)
		return (ft_strdup(""));
	return (val);
}

int	is_space_char(char c)
{
	return (c == ' ' || (c >= 9 && c <= 13));
}

int	is_operator_char(char c)
{
	return (c == '|' || c == '<' || c == '>');
}

int	ft_isalnum(int c)
{
	if ((c >= '0' && c <= '9') || ((c >= 'a' && c <= 'z') || (c >= 'A'
				&& c <= 'Z')))
		return (1);
	return (0);
}

int	append_char_to_buf(char *buf, int *idx, int buf_size, char c)
{
	if (*idx >= buf_size - 1)
	{
		return (-1);
	}
	buf[(*idx)++] = c;
	return (0);
}
