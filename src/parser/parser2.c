/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser2.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/12 11:06:37 by vmamoten          #+#    #+#             */
/*   Updated: 2025/01/12 11:07:40 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

int	handle_argument_token(t_token *tokens, t_exec_command *current_cmd)
{
	if (!current_cmd)
	{
		ft_putstr_fd("Syntax error: argument with no command\n", 2);
		return (0);
	}
	current_cmd->args = add_argument(current_cmd->args, tokens->str);
	return (1);
}

int	process_redirection(t_token **tokens, t_exec_command *current_cmd)
{
	t_redirection_params	params;

	params.type = (*tokens)->type;
	params.is_append = ((*tokens)->type == TOKEN_REDIRECT_APPEND);
	params.is_heredoc = ((*tokens)->type == TOKEN_HEREDOC);
	*tokens = (*tokens)->next;
	if (!(*tokens) || ((*tokens)->type != TOKEN_FILE
			&& (*tokens)->type != TOKEN_HEREDOC_MARKER))
	{
		ft_putstr_fd("Syntax error: expected file after redirection\n", 2);
		return (0);
	}
	params.filename = (*tokens)->str;
	if ((*tokens)->type == TOKEN_HEREDOC_MARKER)
		params.heredoc_marker = (*tokens)->str;
	else
		params.heredoc_marker = NULL;
	current_cmd->redirects = add_redirection(current_cmd->redirects, params);
	return (current_cmd->redirects != NULL);
}

int	handle_redirection_token(t_token **tokens, t_command_context *ctx)
{
	if (!*ctx->current_cmd)
	{
		*ctx->current_cmd = create_command_node();
		if (!*ctx->cmd_list)
			*ctx->cmd_list = *ctx->current_cmd;
		if (*ctx->prev_cmd)
		{
			(*ctx->prev_cmd)->next_cmd = *ctx->current_cmd;
			(*ctx->current_cmd)->prev_cmd = *ctx->prev_cmd;
		}
	}
	return (process_redirection(tokens, *ctx->current_cmd));
}

int	process_single_token(t_token *tokens, t_command_context *ctx)
{
	if (tokens->type == TOKEN_PIPE)
		*ctx->current_cmd = handle_pipe_token(*ctx->current_cmd, ctx);
	else if (tokens->type == TOKEN_COMMAND)
		handle_command_token(tokens, ctx);
	else if (tokens->type == TOKEN_ARGUMENT)
	{
		if (!handle_argument_token(tokens, *ctx->current_cmd))
			return (0);
	}
	else if (tokens->type == TOKEN_REDIRECT_IN
		|| tokens->type == TOKEN_REDIRECT_OUT
		|| tokens->type == TOKEN_REDIRECT_APPEND
		|| tokens->type == TOKEN_HEREDOC)
	{
		if (!handle_redirection_token(&tokens, ctx))
			return (0);
	}
	return (1);
}

t_exec_command	*parse_tokens_to_commands(t_token *tokens)
{
	t_exec_command		*cmd_list;
	t_exec_command		*current_cmd;
	t_exec_command		*prev_cmd;
	t_command_context	ctx;

	cmd_list = NULL;
	current_cmd = NULL;
	prev_cmd = NULL;
	ctx.current_cmd = &current_cmd;
	ctx.cmd_list = &cmd_list;
	ctx.prev_cmd = &prev_cmd;
	while (tokens)
	{
		if (!process_single_token(tokens, &ctx))
			return (NULL);
		tokens = tokens->next;
	}
	if (!cmd_list && current_cmd)
		cmd_list = current_cmd;
	return (cmd_list);
}
