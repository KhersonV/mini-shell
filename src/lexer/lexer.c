/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/12 10:28:26 by vmamoten          #+#    #+#             */
/*   Updated: 2025/01/12 10:41:48 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

void	append_expanded_unquoted(const char *input, int *i,
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

t_token	*tokenizer(char *user_input, t_info *info)
{
	t_lexer_params	params;
	t_token			*head;
	char			buf[1024];
	int				buf_index;
	int				i;

	head = NULL;
	buf_index = 0;
	i = 0;
	params.head = &head;
	params.buf = buf;
	params.buf_index = &buf_index;
	params.input = user_input;
	params.i = &i;
	params.info = info;
	params.buf_size = 1024;
	while (user_input[i] != '\0')
	{
		skip_spaces(&params);
		if (process_token(params, info))
			continue ;
		break ;
	}
	flush_buf_if_needed(&head, buf, &buf_index);
	return (head);
}
