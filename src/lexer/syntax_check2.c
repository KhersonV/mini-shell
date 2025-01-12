/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   syntax_check2.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/12 10:36:36 by vmamoten          #+#    #+#             */
/*   Updated: 2025/01/12 10:37:45 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

int	check_pipes_error(t_token *token, t_info *info)
{
	if (!token->next)
	{
		ft_putstr_fd("minishell: syntax error near unexpected token `|'\n", 2);
		info->syntax_error = 1;
		info->exit_status = 258;
		return (-1);
	}
	if (token->next->type == TOKEN_PIPE)
	{
		ft_putstr_fd("minishell: syntax error near unexpected token `|'\n", 2);
		info->syntax_error = 1;
		info->exit_status = 258;
		return (-1);
	}
	return (0);
}

int	print_syntax_error(char *token_str, t_info *info)
{
	write(2, "minishell: syntax error near unexpected token `", 47);
	write(2, token_str, ft_strlen(token_str));
	write(2, "'\n", 2);
	info->syntax_error = 1;
	info->exit_status = 258;
	return (-1);
}

int	validate_redirect_target(t_token *token, t_info *info)
{
	if (!token->next)
		return (print_syntax_error(token->str, info));
	if (token->next->type == TOKEN_PIPE
		|| token->next->type == TOKEN_REDIRECT_IN
		|| token->next->type == TOKEN_REDIRECT_OUT
		|| token->next->type == TOKEN_REDIRECT_APPEND
		|| token->next->type == TOKEN_HEREDOC)
		return (print_syntax_error(token->next->str, info));
	if (token->next->type != TOKEN_COMMAND
		&& token->next->type != TOKEN_ARGUMENT
		&& token->next->type != TOKEN_WORD && token->next->type != TOKEN_FIELD
		&& token->next->type != TOKEN_EXP_FIELD
		&& token->next->type != TOKEN_VAR
		&& token->next->type != TOKEN_EXIT_STATUS)
		return (print_syntax_error(token->str, info));
	token->next->type = TOKEN_FILE;
	return (0);
}

int	redirect_check(t_token *token, t_info *info)
{
	if (token->type == TOKEN_REDIRECT_IN || token->type == TOKEN_REDIRECT_OUT
		|| token->type == TOKEN_REDIRECT_APPEND || token->type == TOKEN_HEREDOC)
	{
		return (validate_redirect_target(token, info));
	}
	return (0);
}

void	adjust_word_token(t_token *token, int *command)
{
	if (!(*command) && (token->type == TOKEN_WORD
			|| token->type == TOKEN_EXP_FIELD || token->type == TOKEN_FIELD
			|| token->type == TOKEN_VAR))
	{
		token->type = TOKEN_COMMAND;
		(*command) = 1;
	}
	else if ((*command) && (token->type == TOKEN_WORD
			|| token->type == TOKEN_FIELD || token->type == TOKEN_EXP_FIELD
			|| token->type == TOKEN_VAR || token->type == TOKEN_EXIT_STATUS))
		token->type = TOKEN_ARGUMENT;
}
