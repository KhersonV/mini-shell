
#include "../../include/minishell.h"

int	check_pipes_error(t_token *token, t_info *info)
{
	if (!token->next)
	{
		fprintf(stderr, "minishell: syntax error near unexpected token `|'\n");
		info->syntax_error = 1;
		info->exit_status = 258;
		return (-1);
	}
	if (token->next->type == TOKEN_PIPE)
	{
		fprintf(stderr, "minishell: syntax error near unexpected token `|'\n");
		info->syntax_error = 1;
		info->exit_status = 258;
		return (-1);
	}
	return (0);
}

int	print_syntax_error(char *token_str, t_info *info)
{
	write(2, "minishell: syntax error near unexpected token `", 47);
	write(2, token_str, strlen(token_str));
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

void	adjusting_token_tree(t_token **tree, t_info *info)
{
	t_token *curr;
	int command_found;

	command_found = 0;
	info->syntax_error = 0;
	if (!tree || !*tree)
		return ;
	if ((*tree)->type == TOKEN_PIPE)
	{
		fprintf(stderr, "minishell: syntax error near unexpected token `|'\n");
		info->syntax_error = 1;
		info->exit_status = 2;
		return ;
	}
	curr = *tree;
	while (curr)
	{
		if (curr->type == TOKEN_PIPE)
		{
			command_found = 0;
			if (check_pipes_error(curr, info) == -1)
				return ;
		}
		adjust_word_token(curr, &command_found);
		if (redirect_check(curr, info) == -1)
			return ;
		curr = curr->next;
	}
}
