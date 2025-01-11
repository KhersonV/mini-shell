
#include "../../include/minishell.h"

// char		*expand_variable(char *var_name, t_info *info);
char	*expand_dollar(const char *input, int *consumed, t_info *info);
char	*ft_expand_variable(char *var_name, t_info *info);
int	is_space_char(char c);
int	is_operator_char(char c);
int	ft_isalnum(int c);
int	append_char_to_buf(char *buf, int *idx, int buf_size, char c);
int	read_single_quoted(const char *input, char *buf, int *buf_index,
		int buf_size);
t_token	*create_token_node(char *name, int type);
int read_dollar_single(const char *input, char *buf, int *buf_index);
int handle_bslash_short(const char **p, char *buf, int *buf_index);
int	handle_bslash_long(const char **p, char *buf, int *buf_index);
int	handle_dquotes_backslash(const char **p, char *buf, int *buf_index);
int handle_dquotes_dollar(const char **p, char *buf, int *buf_index,
								 t_info *info);
int	handle_dquotes_normal_char(const char **p, char *buf,
									  int *buf_index);
int	read_dollar_double(const char *input, char *buf, int *buf_index,
							  t_info *info);
char *read_dollar_quoted(const char *input, int *consumed, t_info *info);

/************* */
int is_early_termination(char next_char, char quote);
int find_closing_quote(const char *input, char quote);
char *handle_no_closing_quote(int *consumed);
char *handle_quoted_dollar(const char *input, int *consumed);
char	*expand_dollar(const char *input, int *consumed, t_info *info);

/***********/


//****************** */

t_token	*add_token(t_token *node, char *name, int type);
int	is_quotes_closed(const char *start);
void	flush_buf_if_needed(t_token **curr, char *buf, int *buf_index);


t_token	*add_operator_token(t_token *curr, char current_char, char next_char,
		int *i)
{
	if (current_char == '|')
		curr = add_token(curr, "|", TOKEN_PIPE);
	else if (current_char == '<')
	{
		if (next_char == '<')
		{
			curr = add_token(curr, "<<", TOKEN_HEREDOC);
			(*i)++;
		}
		else
			curr = add_token(curr, "<", TOKEN_REDIRECT_IN);
	}
	else if (current_char == '>')
	{
		if (next_char == '>')
		{
			curr = add_token(curr, ">>", TOKEN_REDIRECT_APPEND);
			(*i)++;
		}
		else
			curr = add_token(curr, ">", TOKEN_REDIRECT_OUT);
	}
	return curr;
}

static void	append_expanded_unquoted(const char *input, int *i, char *buf,
		int *buf_index, int buf_size, t_info *info)
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
	len = strlen(expanded);
	while (k < len)
	{
		if (append_char_to_buf(buf, buf_index, buf_size, expanded[k]) < 0)
		{
			free(expanded);
			return ;
		}
		k++;
	}
	free(expanded);
	*i = *i + var_consumed;
}

static void	append_expanded_dquotes(const char *input, int *i, char *buf,
		int *buf_index, int buf_size, t_info *info)
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
	len = strlen(expanded);
	while (k < len)
	{
		if (append_char_to_buf(buf, buf_index, buf_size, expanded[k]) < 0)
		{
			free(expanded);
			return ;
		}
		k++;
	}
	free(expanded);
	*i = *i + var_consumed;
}

static int	read_double_quoted(const char *input, char *buf, int *buf_index,
		int buf_size, t_info *info)
{
	int	i;

	i = 1;
	while (input[i] && input[i] != '"')
	{
		if (input[i] == '\\')
		{
			i++;
			if (!input[i])
				break ;
			if (input[i] == '"' || input[i] == '$' || input[i] == '\\')
			{
				if (append_char_to_buf(buf, buf_index, buf_size, input[i]) < 0)
					return i;
				i++;
			}
			else
			{
				if (append_char_to_buf(buf, buf_index, buf_size, '\\') < 0)
					return i;
				if (append_char_to_buf(buf, buf_index, buf_size, input[i]) < 0)
					return i;
				i++;
			}
		}
		else if (input[i] == '$')
		{
			append_expanded_dquotes(input, &i, buf, buf_index, buf_size, info);
			continue ;
		}
		else
		{
			if (append_char_to_buf(buf, buf_index, buf_size, input[i]) < 0)
				return i;
			i++;
		}
	}
	if (input[i] == '"')
		i++;
	return i;
}

static int	read_unquoted(const char *input, char *buf, int *buf_index,
		int buf_size, t_info *info)
{
	int	i;

	i = 0;
	while (input[i] != '\0')
	{
		if (is_space_char(input[i]) || is_operator_char(input[i]))
			break ;
		if (input[i] == '\'' || input[i] == '"')
			break ;
		if (input[i] == '$')
		{
			append_expanded_unquoted(input, &i, buf, buf_index, buf_size, info);
			continue ;
		}
		if (input[i] == '\\')
		{
			i++;
			if (!input[i])
				break ;
			if (strchr("$\\\"'", input[i]))
			{
				if (append_char_to_buf(buf, buf_index, buf_size, input[i]) < 0)
					return i;
				i++;
			}
			else
			{
				if (append_char_to_buf(buf, buf_index, buf_size, '\\') < 0)
					return i;
				if (append_char_to_buf(buf, buf_index, buf_size, input[i]) < 0)
					return i;
				i++;
			}
			continue ;
		}
		if (append_char_to_buf(buf, buf_index, buf_size, input[i]) < 0)
			return i;
		i++;
	}
	return i;
}

static void	skip_spaces(t_token **head, char *buf, int *buf_index,
		const char *user_input, int *i)
{
	while (is_space_char(user_input[*i]))
	{
		flush_buf_if_needed(head, buf, buf_index);
		*i = *i + 1;
	}
}

static int	handle_double_quote(t_token **head, char *buf, int *buf_index,
		const char *user_input, int *i, t_info *info)
{
	int		old_index;
	int		consumed;
	char	c;

	if (user_input[*i] != '"')
		return 0;
	old_index = *buf_index;
	consumed = read_double_quoted(&user_input[*i], buf, buf_index, 1024, info);
	*i = *i + consumed;
	if (*buf_index == old_index)
	{
		c = user_input[*i];
		if (c == '\0' || is_space_char(c) || is_operator_char(c))
		{
			buf[*buf_index] = '\0';
			*buf_index = *buf_index + 1;
			flush_buf_if_needed(head, buf, buf_index);
		}
	}
	return 1;
}

static int	handle_operator_char(t_token **head, char *buf, int *buf_index,
		const char *user_input, int *i)
{
	if (!is_operator_char(user_input[*i]))
		return 0;
	flush_buf_if_needed(head, buf, buf_index);
	*head = add_operator_token(*head, user_input[*i], user_input[*i + 1], i);
	*i = *i + 1;
	return 1;
}

static int	handle_single_quote(t_token **head, char *buf, int *buf_index,
		const char *user_input, int *i)
{
	int		old_index;
	int		consumed;
	char	c;

	if (user_input[*i] != '\'')
		return 0;
	old_index = *buf_index;
	consumed = read_single_quoted(&user_input[*i], buf, buf_index, 1024);
	*i = *i + consumed;
	if (*buf_index == old_index)
	{
		c = user_input[*i];
		if (c == '\0' || is_space_char(c) || is_operator_char(c))
		{
			buf[*buf_index] = '\0';
			*buf_index = *buf_index + 1;
			flush_buf_if_needed(head, buf, buf_index);
		}
	}
	return 1;
}

static int	handle_unquoted(char *buf, int *buf_index, const char *user_input,
		int *i, t_info *info)
{
	int	consumed;

	if (is_space_char(user_input[*i]) || is_operator_char(user_input[*i])
		|| user_input[*i] == '\'' || user_input[*i] == '"')
	{
		return 0;
	}
	consumed = read_unquoted(&user_input[*i], buf, buf_index, 1024, info);
	*i = *i + consumed;
	return 1;
}

t_token	*tokenizer(char *user_input, t_info *info)
{
	t_token	*head;
	char	buf[1024];
	int		buf_index;
	int		i;
	int		handled;

	head = NULL;
	buf_index = 0;
	i = 0;
	while (user_input[i] != '\0')
	{
		skip_spaces(&head, buf, &buf_index, user_input, &i);
		handled = handle_operator_char(&head, buf, &buf_index, user_input, &i);
		if (handled)
			continue ;
		handled = handle_single_quote(&head, buf, &buf_index, user_input, &i);
		if (handled)
			continue ;
		handled = handle_double_quote(&head, buf, &buf_index, user_input, &i,
				info);
		if (handled)
			continue ;
		handled = handle_unquoted(buf, &buf_index, user_input, &i, info);
		if (!handled)
			break ;
	}
	flush_buf_if_needed(&head, buf, &buf_index);
	return head;
}
