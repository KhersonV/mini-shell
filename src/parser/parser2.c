
#include "../../include/minishell.h"

void			remove_space_tokens(t_token **tree);
void			adjusting_token_tree(t_token **tree, t_info *info);

void			temp_print_tokens(t_token *node);
int				validate_syntax_and_adjust(t_token **tree);

char	**add_argument(char **args, const char *arg)
{
	int		count;
	char	**new_args;

	count = 0;
	while (args && args[count])
		count++;
	new_args = malloc(sizeof(char *) * (count + 2));
	if (!new_args)
		return (NULL);
	for (int i = 0; i < count; i++)
		new_args[i] = args[i];
	new_args[count] = strdup(arg);
	new_args[count + 1] = NULL;
	free(args);
	return (new_args);
}

t_redirection	*add_redirection(t_redirection *redirects, const char *filename,
		int type, int is_append, int is_heredoc, const char *heredoc_marker)
{
	t_redirection	*new_redir;

	new_redir = malloc(sizeof(t_redirection));
	if (!new_redir)
		return (NULL);
	new_redir->filename = strdup(filename);
	new_redir->type = type;
	new_redir->is_append = is_append;
	new_redir->is_heredoc = is_heredoc;
	new_redir->heredoc_marker = heredoc_marker ? strdup(heredoc_marker) : NULL;
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

t_exec_command	*parse_tokens_to_commands(t_token *tokens)
{
	t_exec_command	*cmd_list;
	t_exec_command	*current_cmd;
	t_exec_command	*prev_cmd;
	t_exec_command	*new_cmd;
	t_redirection	*redir;

	cmd_list = NULL;
	current_cmd = NULL;
	prev_cmd = NULL;
	while (tokens)
	{
		if (tokens->type == TOKEN_PIPE)
		{
			if (!current_cmd)
			{
				fprintf(stderr, "Syntax error: unexpected pipe\n");
				return (NULL);
			}
			new_cmd = create_command_node();
			current_cmd->next_cmd = new_cmd;
			new_cmd->prev_cmd = current_cmd;
			if (!cmd_list)
				cmd_list = current_cmd;
			prev_cmd = current_cmd;
			current_cmd = new_cmd;
		}
		else if (tokens->type == TOKEN_COMMAND)
		{
			if (!current_cmd)
			{
				current_cmd = create_command_node();
				if (!cmd_list)
					cmd_list = current_cmd;
				if (prev_cmd)
				{
					prev_cmd->next_cmd = current_cmd;
					current_cmd->prev_cmd = prev_cmd;
				}
			}
			if (!current_cmd->cmd_name)
				current_cmd->cmd_name = strdup(tokens->str);
			current_cmd->args = add_argument(current_cmd->args, tokens->str);
		}
		else if (tokens->type == TOKEN_ARGUMENT)
		{
			if (!current_cmd)
			{
				fprintf(stderr, "Syntax error: argument with no command\n");
				return (NULL);
			}
			current_cmd->args = add_argument(current_cmd->args, tokens->str);
		}
		else if (tokens->type == TOKEN_REDIRECT_IN
			|| tokens->type == TOKEN_REDIRECT_OUT
			|| tokens->type == TOKEN_REDIRECT_APPEND
			|| tokens->type == TOKEN_HEREDOC)
		{
			if (!current_cmd)
			{
				current_cmd = create_command_node();
				if (!cmd_list)
					cmd_list = current_cmd;
				if (prev_cmd)
				{
					prev_cmd->next_cmd = current_cmd;
					current_cmd->prev_cmd = prev_cmd;
				}
			}
			redir = malloc(sizeof(t_redirection));
			if (!redir)
			{
				perror("malloc");
				return (NULL);
			}
			memset(redir, 0, sizeof(t_redirection));
			redir->type = tokens->type;
			redir->is_append = (tokens->type == TOKEN_REDIRECT_APPEND);
			redir->is_heredoc = (tokens->type == TOKEN_HEREDOC);
			tokens = tokens->next;
			if (!tokens || (tokens->type != TOKEN_FILE
					&& tokens->type != TOKEN_HEREDOC_MARKER))
			{
				fprintf(stderr,
					"Syntax error: expected file after redirection\n");
				free(redir);
				return (NULL);
			}
			redir->filename = strdup(tokens->str);
			redir->next = current_cmd->redirects;
			current_cmd->redirects = redir;
		}
		tokens = tokens->next;
	}
	if (!cmd_list && current_cmd)
		cmd_list = current_cmd;
	return (cmd_list);
}

void	print_command_list(t_exec_command *cmd_list)
{
	t_exec_command	*cmd;
	int				cmd_num;
	t_redirection	*redir;

	cmd = cmd_list;
	cmd_num = 1;
	while (cmd)
	{
		printf("Command #%d:\n", cmd_num++);
		printf("  Command Name: %s\n",
			cmd->cmd_name ? cmd->cmd_name : "(null)");
		if (cmd->args)
		{
			printf("  Arguments:\n");
			for (int i = 0; cmd->args[i]; i++)
				printf("    [%d] %s\n", i, cmd->args[i]);
		}
		else
		{
			printf("  Arguments: None\n");
		}
		if (cmd->redirects)
		{
			printf("  Redirections:\n");
			redir = cmd->redirects;
			while (redir)
			{
				printf("    Type: %s, File: %s%s\n",
					(redir->type == TOKEN_REDIRECT_IN) ? "INPUT" : (redir->type == TOKEN_REDIRECT_OUT) ? "OUTPUT" : (redir->type == TOKEN_REDIRECT_APPEND) ? "APPEND" : (redir->type == TOKEN_HEREDOC) ? "HEREDOC" : "UNKNOWN",
					redir->filename, redir->is_heredoc ? " (Heredoc)" : "");
				redir = redir->next;
			}
		}
		else
		{
			printf("  Redirections: None\n");
		}
		printf("  Exit Status: %d\n\n", cmd->exit_status);
		cmd = cmd->next_cmd;
	}
}
