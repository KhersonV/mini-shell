
#include "../../include/minishell.h"

char **add_argument(char **args, const char *arg) {
	int count = 0;
	while (args && args[count])
		count++;
	char **new_args = malloc(sizeof(char *) * (count + 2));
	if (!new_args)
		return NULL;
	for (int i = 0; i < count; i++)
		new_args[i] = args[i];
	new_args[count] = strdup(arg);
	new_args[count + 1] = NULL;
	free(args);
	return new_args;
}

void remove_spaces(t_token **tree)
{
	t_token *curr;
	t_token *node_to_remove;

	curr = *tree;
	while (curr != NULL)
	{
		if (curr->type == TOKEN_SPACE)
		{
			node_to_remove = curr;
			if (node_to_remove == *tree)
			{
				*tree = node_to_remove->next;
				if (*tree != NULL)
					(*tree)->prev = NULL;
			}
			else
			{
				if (node_to_remove->prev != NULL)
					node_to_remove->prev->next = node_to_remove->next;
				if (node_to_remove->next != NULL)
					node_to_remove->next->prev = node_to_remove->prev;
			}
			curr = curr->next;
			free(node_to_remove);
		}
		else
		{
			curr = curr->next;
		}
	}
}


t_redirection *add_redirection(t_redirection *redirects, const char *filename, int type, int is_append, int is_heredoc, const char *heredoc_marker) {
	t_redirection *new_redir = malloc(sizeof(t_redirection));
	if (!new_redir)
		return NULL;
	new_redir->filename = strdup(filename);
	new_redir->type = type;
	new_redir->is_append = is_append;
	new_redir->is_heredoc = is_heredoc;
	new_redir->heredoc_marker = heredoc_marker ? strdup(heredoc_marker) : NULL;
	new_redir->fd = -1;
	new_redir->next = redirects;
	return new_redir;
}

t_exec_command *create_command_node() {
	t_exec_command *cmd = malloc(sizeof(t_exec_command));
	if (!cmd)
		return NULL;
	cmd->cmd_name = NULL;
	cmd->exec_path = NULL;
	cmd->args = NULL;
	cmd->redirects = NULL;
	cmd->exit_status = 0;
	cmd->next_cmd = NULL;
	cmd->prev_cmd = NULL;
	return cmd;
}

t_exec_command *parse_tokens_to_commands(t_token *tokens)
{
	t_exec_command *cmd_list = NULL;
	t_exec_command *current_cmd = NULL;
	t_exec_command *prev_cmd = NULL;

	while (tokens)
	{
		if (tokens->type == TOKEN_PIPE)
		{
			if (!current_cmd)
			{
				fprintf(stderr, "Syntax error: unexpected pipe\n");
				return NULL;
			}
			

			t_exec_command *new_cmd = create_command_node();
			

			// Link them
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
				return NULL;
			}
			current_cmd->args = add_argument(current_cmd->args, tokens->str);
		}
		// 4) Redirections
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
			t_redirection *redir = malloc(sizeof(t_redirection));
			if (!redir)
			{
				perror("malloc");
				return NULL;
			}
			memset(redir, 0, sizeof(t_redirection));

			redir->type = tokens->type;
			redir->is_append  = (tokens->type == TOKEN_REDIRECT_APPEND);
			redir->is_heredoc = (tokens->type == TOKEN_HEREDOC);

			tokens = tokens->next;
			if (!tokens
			  || (tokens->type != TOKEN_FILE && tokens->type != TOKEN_HEREDOC_MARKER))
			{
				fprintf(stderr, "Syntax error: expected file after redirection\n");
				free(redir);
				return NULL;
			}
			redir->filename = strdup(tokens->str);

			redir->next = current_cmd->redirects;
			current_cmd->redirects = redir;
		}

		// Move to next token
		tokens = tokens->next;
	}

	if (!cmd_list && current_cmd)
		cmd_list = current_cmd;

	return cmd_list;
}

void print_command_list(t_exec_command *cmd_list)
{
	t_exec_command *cmd = cmd_list;
	while (cmd)
	{
		printf("Command: %s\n", cmd->cmd_name ? cmd->cmd_name : "(null)");
		if (cmd->args)
		{
			printf("Arguments:\n");
			for (int i = 0; cmd->args[i]; i++)
				printf("  %s\n", cmd->args[i]);
		}
		if (cmd->redirects)
		{
			printf("Redirections:\n");
			t_redirection *redir = cmd->redirects;
			while (redir)
			{
				printf("  Type: %d, File: %s, Append: %d, Heredoc: %d\n",
					   redir->type, redir->filename,
					   redir->is_append, redir->is_heredoc);
				redir = redir->next;
			}
		}
		// Simple check: if pipe_fds[1] != -1 => it pipes out
		// if pipe_fds[0] != -1 => it pipes in

		printf("Exit Status: %d\n\n", cmd->exit_status);
		cmd = cmd->next_cmd;
	}
}
void adjusting_token_tree(t_token **tree)
{
	t_token *curr;
	int command_found;

	curr = *tree;
	command_found = 0;
	while (curr != NULL)
	{
		if (curr->type == TOKEN_PIPE)
		{
			command_found = 0;
		}
		if (!command_found && curr->type == TOKEN_WORD)
		{
			curr->type = TOKEN_COMMAND;
			command_found = 1;
		}
		else if (command_found && (curr->type == TOKEN_WORD || curr->type == TOKEN_FIELD ||
								   curr->type == TOKEN_EXP_FIELD || curr->type == TOKEN_VAR))
		{
			curr->type = TOKEN_ARGUMENT;
		}
		if (curr->type == TOKEN_REDIRECT_IN || curr->type == TOKEN_REDIRECT_OUT ||
			curr->type == TOKEN_REDIRECT_APPEND)
		{
			if (curr->next != NULL)
				curr->next->type = TOKEN_FILE;
		}
		else if (curr->type == TOKEN_HEREDOC)
		{
			if (curr->next != NULL)
				curr->next->type = TOKEN_HEREDOC_MARKER;
		}
		curr = curr->next;
	}
}

// int main()
// {
// 	char *inputs = "echo 'Hello World' | grep Hello >> output.txt | wc -l < input.txt";

// 		t_token *tokens = NULL;

// 		tokens = tokenize(inputs);

// 		remove_spaces(&tokens);
// 		adjusting_token_tree(&tokens);

// 		t_exec_command *commands = parse_tokens_to_commands(tokens);

// 		print_command_list(commands);

// 	return 0;
// }
