/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/12 11:06:27 by vmamoten          #+#    #+#             */
/*   Updated: 2025/01/12 11:07:31 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

char	**add_argument(char **args, const char *arg)
{
	int		count;
	int		i;
	char	**new_args;

	count = 0;
	while (args && args[count])
		count++;
	new_args = malloc(sizeof(char *) * (count + 2));
	if (!new_args)
		return (NULL);
	i = 0;
	while (i < count)
	{
		new_args[i] = args[i];
		i++;
	}
	new_args[count] = ft_strdup(arg);
	new_args[count + 1] = NULL;
	free(args);
	return (new_args);
}

t_redirection	*add_redirection(t_redirection *redirects,
		t_redirection_params params)
{
	t_redirection	*new_redir;

	new_redir = malloc(sizeof(t_redirection));
	if (!new_redir)
		return (NULL);
	new_redir->filename = ft_strdup(params.filename);
	new_redir->type = params.type;
	new_redir->is_append = params.is_append;
	new_redir->is_heredoc = params.is_heredoc;
	if (params.heredoc_marker)
		new_redir->heredoc_marker = ft_strdup(params.heredoc_marker);
	else
		new_redir->heredoc_marker = NULL;
	new_redir->fd = -1;
	new_redir->next = redirects;
	return (new_redir);
}

t_exec_command	*create_command_node(void)
{
	t_exec_command	*cmd;

	cmd = malloc(sizeof(t_exec_command));
	if (!cmd)
		return (NULL);
	cmd->cmd_name = NULL;
	cmd->exec_path = NULL;
	cmd->args = NULL;
	cmd->redirects = NULL;
	cmd->exit_status = 0;
	cmd->next_cmd = NULL;
	cmd->prev_cmd = NULL;
	return (cmd);
}

t_exec_command	*handle_pipe_token(t_exec_command *current_cmd,
		t_command_context *ctx)
{
	t_exec_command	*new_cmd;

	if (!current_cmd)
	{
		ft_putstr_fd("Syntax error: unexpected pipe\n", 2);
		return (NULL);
	}
	new_cmd = create_command_node();
	current_cmd->next_cmd = new_cmd;
	new_cmd->prev_cmd = current_cmd;
	if (!*ctx->cmd_list)
		*ctx->cmd_list = current_cmd;
	*ctx->prev_cmd = current_cmd;
	return (new_cmd);
}

void	handle_command_token(t_token *tokens, t_command_context *ctx)
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
	if (!(*ctx->current_cmd)->cmd_name)
		(*ctx->current_cmd)->cmd_name = ft_strdup(tokens->str);
	(*ctx->current_cmd)->args = add_argument((*ctx->current_cmd)->args,
			tokens->str);
}
