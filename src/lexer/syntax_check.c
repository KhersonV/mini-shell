
#include "../../include/minishell.h"

void assign_roles(t_token **tree)
{
	t_token *curr;
	int command_found;

	command_found = 0;
	curr = *tree;
	while (curr)
	{
		if (curr->type == TOKEN_PIPE)
			command_found = 0;
		if (curr->type == TOKEN_REDIRECT_IN ||
			curr->type == TOKEN_REDIRECT_OUT ||
			curr->type == TOKEN_REDIRECT_APPEND)
		{
			if (curr->next && curr->next->type == TOKEN_WORD)
				curr->next->type = TOKEN_FILE;
		}
		if (curr->type == TOKEN_HEREDOC)
		{
			if (curr->next && curr->next->type == TOKEN_WORD)
				curr->next->type = TOKEN_HEREDOC_MARKER;
		}
		if (!command_found && curr->type == TOKEN_WORD)
		{
			curr->type = TOKEN_COMMAND;
			command_found = 1;
		}
		else if (command_found &&
				(curr->type == TOKEN_WORD || curr->type == TOKEN_FIELD ||
				 curr->type == TOKEN_EXP_FIELD || curr->type == TOKEN_VAR))
		{
			curr->type = TOKEN_ARGUMENT;
		}
		curr = curr->next;
	}
}

int check_pipe_errors(t_token *tokens)
{
	t_token *tail;
	t_token *curr;

	if (!tokens) return 0;

	if (tokens->type == TOKEN_PIPE)
	{
		printf("Syntax error: pipe at start\n");
		return 1;
	}
	curr = tokens;
	while (curr)
	{
		if (curr->type == TOKEN_PIPE)
		{
			if (!curr->next || curr->next->type == TOKEN_PIPE)
			{
				printf("Syntax error: pipe misplaced\n");
				return 1;
			}
		}
		curr = curr->next;
	}
	tail = tokens;
	while (tail->next) tail = tail->next;
	if (tail->type == TOKEN_PIPE)
	{
		printf("Syntax error: pipe at end\n");
		return 1;
	}
	return 0;
}

int check_redirect_errors(t_token *tokens)
{
	t_token *curr;

	if (!tokens)
		return 0;
	curr = tokens;

	while (curr)
	{
		if (curr->type == TOKEN_REDIRECT_IN ||
			curr->type == TOKEN_REDIRECT_OUT ||
			curr->type == TOKEN_REDIRECT_APPEND)
		{

			if (!curr->next ||
				curr->next->type == TOKEN_PIPE ||
				curr->next->type == TOKEN_REDIRECT_IN ||
				curr->next->type == TOKEN_REDIRECT_OUT ||
				curr->next->type == TOKEN_REDIRECT_APPEND)
			{
				printf("Syntax error: missing or misplaced file\n");
				return 1;
			}
		}
		if (curr->type == TOKEN_REDIRECT_IN &&
			curr->next && curr->next->type == TOKEN_REDIRECT_OUT)
		{
			printf("Syntax error: unexpected token '>'\n");
			return 1;
		}
		curr = curr->next;
	}
	return 0;
}

int validate_syntax_and_adjust(t_token **tree)
{
	t_token *curr;
	int has_command;

	assign_roles(tree);
	has_command = 0;
	curr = *tree;
	while (curr)
	{
		if (curr->type == TOKEN_COMMAND)
		{
			has_command = 1;
			break;
		}
		curr = curr->next;
	}

	if (check_pipe_errors(*tree))
		return 1;

	if (check_redirect_errors(*tree))
		return 1;

	if (!has_command && !*tree)
	{
		printf("Syntax error: no command or redirection\n");
		return 1;
	}
	return 0;
}