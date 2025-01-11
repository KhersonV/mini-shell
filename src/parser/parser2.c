
#include "../../include/minishell.h"


typedef struct s_command_context
{
	t_exec_command	**current_cmd;
	t_exec_command	**cmd_list;
	t_exec_command	**prev_cmd;
}					t_command_context;

typedef struct s_redirection_params
{
	const char		*filename;
	int				type;
	int				is_append;
	int				is_heredoc;
	const char		*heredoc_marker;
}					t_redirection_params;

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
