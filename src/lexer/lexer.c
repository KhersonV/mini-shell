
#include "../../include/minishell.h"

// char		*expand_variable(char *var_name, t_info *info);
static char	*expand_dollar(const char *input, int *consumed, t_info *info);

char	*ft_expand_variable(char *var_name, t_info *info)
{
	char	*val;

	val = get_env_value(info, var_name);
	if (val == NULL)
		return (ft_strdup(""));
	return (ft_strdup(val));
}

void		expansion(t_token **tokens, t_info *info);

static int	is_space_char(char c)
{
	return (c == ' ' || (c >= 9 && c <= 13));
}

static int	is_operator_char(char c)
{
	return (c == '|' || c == '<' || c == '>');
}

int	ft_isalnum(int c)
{
	if ((c >= '0' && c <= '9') || ((c >= 'a' && c <= 'z') || (c >= 'A'
				&& c <= 'Z')))
		return (1);
	return (0);
}

char	*print_token(int current_token)
{
	switch (current_token)
	{
	case TOKEN_SPACE:
		return ("TOKEN_SPACE");
	case TOKEN_FILE:
		return ("TOKEN_FILE");
	case TOKEN_HEREDOC:
		return ("TOKEN_HEREDOC");
	case TOKEN_COMMAND:
		return ("TOKEN_COMMAND");
	case TOKEN_ARGUMENT:
		return ("TOKEN_ARGUMENT");
	case TOKEN_REDIRECT_APPEND:
		return ("TOKEN_REDIRECT_APPEND");
	case TOKEN_PIPE:
		return ("TOKEN_PIPE");
	case TOKEN_REDIRECT_OUT:
		return ("TOKEN_REDIRECT_OUT");
	case TOKEN_REDIRECT_IN:
		return ("TOKEN_REDIRECT_IN");
	case TOKEN_WORD:
		return ("TOKEN_WORD");
	case TOKEN_VAR:
		return ("TOKEN_VAR");
	case TOKEN_EXP_FIELD:
		return ("TOKEN_EXP_FIELD");
	case TOKEN_FIELD:
		return ("TOKEN_FIELD");
	case TOKEN_HEREDOC_MARKER:
		return ("TOKEN_HEREDOC_MARKER");
	case TOKEN_EXIT_STATUS:
		return ("TOKEN_EXIT_STATUS");
	default:
		return ("UNKNOWN_TYPE");
	}
}

static int	append_char_to_buf(char *buf, int *idx, int buf_size, char c)
{
	if (*idx >= buf_size - 1)
	{
		return (-1);
	}
	buf[(*idx)++] = c;
	return (0);
}

static int	read_single_quoted(const char *input, char *buf, int *buf_index,
		int buf_size)
{
	int	i;

	i = 1;
	while (input[i] && input[i] != '\'')
	{
		if (append_char_to_buf(buf, buf_index, buf_size, input[i]) < 0)
		{
			fprintf(stderr, "Buffer overflow in single quotes\n"); //todo
			return (i);
		}
		i++;
	}
	if (input[i] == '\'')
	{
		i++;
	}
	return (i);
}

t_token	*create_token_node(char *name, int type)
{
	t_token	*new_node;

	new_node = (t_token *)malloc(sizeof(t_token));
	if (!new_node)
		return (NULL);
	new_node->str = ft_strdup(name);
	new_node->type = type;
	new_node->next = NULL;
	new_node->prev = NULL;
	return (new_node);
}

static int read_dollar_single(const char *input, char *buf, int *buf_index)
{
	int i;
	int stop;

	i = 2;
	stop = 0;
	while (input[i] && input[i] != '\'' && stop == 0)
	{
		if (append_char_to_buf(buf, buf_index, 1024, input[i]) < 0)
		{
			fprintf(stderr, "Buffer overflow in $'...'\n"); // todo
			stop = 1;
		}
		else
			i++;
	}
	if (stop == 0 && input[i] == '\'')
		i++;
	return (i);
}

static int handle_bslash_short(const char **p, char *buf, int *buf_index)
{
	int stop;
	int res;

	stop = 0;
	res = append_char_to_buf(buf, buf_index, 1024, (*p)[0]);
	if (res < 0)
		stop = 1;
	*p = (*p) + 1;
	return (stop);
}

static int handle_bslash_long(const char **p, char *buf, int *buf_index)
{
	int local_stop;
	int res;

	local_stop = 0;
	res = append_char_to_buf(buf, buf_index, 1024, '\\');
	if (res < 0)
		local_stop = 1;

	if (local_stop == 0)
	{
		res = append_char_to_buf(buf, buf_index, 1024, (*p)[0]);
		if (res < 0)
			local_stop = 1;
	}
	*p = (*p) + 1;
	return local_stop;
}

static int handle_dquotes_backslash(const char **p, char *buf, int *buf_index)
{
	int stop;
	int check;

	stop = 0;
	*p = (*p) + 1;
	if ((*p)[0] == '\0')
		return stop;

	if (ft_strchr("\"$\\", (*p)[0]))
	{
		check = handle_bslash_short(p, buf, buf_index);
		if (check != 0)
			stop = 1;
	}
	else
	{
		check = handle_bslash_long(p, buf, buf_index);
		if (check != 0)
			stop = 1;
	}
	return (stop);
}

static int handle_dquotes_dollar(const char **p, char *buf, int *buf_index,
								 t_info *info)
{
	int stop;
	int var_consumed;
	char *expanded;
	int copy_stop;

	stop = 0;
	var_consumed = 0;
	expanded = expand_dollar(*p, &var_consumed, info);
	if (!expanded)
		stop = 1;
	else
	{
		copy_stop = 0;
		while (expanded[copy_stop] != '\0' && stop == 0)
		{
			if (append_char_to_buf(buf, buf_index, 1024, expanded[copy_stop]) < 0)
				stop = 1;
			else
				copy_stop = copy_stop + 1;
		}
		free(expanded);
		*p = (*p) + var_consumed;
	}
	return (stop);
}

static int	handle_dquotes_normal_char(const char **p, char *buf,
									  int *buf_index)
{
	int stop;
	int res;

	stop = 0;
	res = append_char_to_buf(buf, buf_index, 1024, (*p)[0]);
	if (res < 0)
		stop = 1;
	else
		*p = (*p) + 1;
	return (stop);
}

static int	read_dollar_double(const char *input, char *buf, int *buf_index,
							  t_info *info)
{
	const char *p;
	const char *start;
	int stop;

	start = &input[2];
	p = start;
	stop = 0;
	while (p[0] && p[0] != '"' && stop == 0)
	{
		if (p[0] == '\\')
			stop = handle_dquotes_backslash(&p, buf, buf_index);
		else if (p[0] == '$')
			stop = handle_dquotes_dollar(&p, buf, buf_index, info);
		else
			stop = handle_dquotes_normal_char(&p, buf, buf_index);
	}
	if (stop == 0 && p[0] == '"')
		p++;
	return (p - input);
}

static char *read_dollar_quoted(const char *input, int *consumed, t_info *info)
{
	char quote;
	char buf[1024];
	int buf_index;
	int i;

	quote = input[1];
	buf_index = 0;
	i = 0;
	if (quote == '\'')
		i = read_dollar_single(input, buf, &buf_index);
	else if (quote == '"')
		i = read_dollar_double(input, buf, &buf_index, info);

	buf[buf_index] = '\0';
	*consumed = i;
	return (ft_strdup(buf));
}

/************* */

static char	*handle_quoted_dollar(const char *input, int *consumed)
{
	char	quote;
	char	next_char;
	int		j;
	int		found_closing_quote;

	quote = input[1];
	next_char = input[2];
	if (next_char == '\0' || is_space_char(next_char)
		|| is_operator_char(next_char) || next_char == quote)
	{
		*consumed = 1;
		return (strdup("$"));
	}
	j = 2;
	found_closing_quote = 0;
	while (input[j])
	{
		if (input[j] == quote)
		{
			found_closing_quote = 1;
			break ;
		}
		j++;
	}
	if (!found_closing_quote)
	{
		*consumed = 1;
		return (strdup("$"));
	}
	return (NULL);
}

static char	*handle_special_variable(const char *var_name, t_info *info)
{
	if (strcmp(var_name, "?") == 0)
	{
		return (ft_itoa(info->exit_status));
	}
	if (strcmp(var_name, "$") == 0)
	{
		return (strdup("$"));
	}
	return NULL;
}

static char	*handle_variable_expansion(const char *input, int *consumed,
		t_info *info)
{
	int		var_consumed;
	char	*var_name;
	char	*expanded;

	var_consumed = 0;
	var_name = read_var_name(input, &var_consumed);
	if (!var_name)
	{
		*consumed = 0;
		return strdup("");
	}
	*consumed = var_consumed;
	expanded = handle_special_variable(var_name, info);
	if (expanded)
	{
		free(var_name);
		return expanded;
	}
	expanded = ft_expand_variable(var_name, info);
	free(var_name);
	return expanded;
}

static char	*expand_dollar(const char *input, int *consumed, t_info *info)
{
	char	*result;

	if (input[1] == '\'' || input[1] == '"')
	{
		result = handle_quoted_dollar(input, consumed);
		if (result)
			return result;
		return read_dollar_quoted(input, consumed, info);
	}
	return handle_variable_expansion(input, consumed, info);
}

//****************** */

t_token	*add_token(t_token *node, char *name, int type)
{
	t_token	*new_node;
	t_token	*curr;

	new_node = create_token_node(name, type);
	if (!new_node)
		return (NULL);
	if (!node)
		return (new_node);
	curr = node;
	while (curr->next)
	{
		curr = curr->next;
	}
	curr->next = new_node;
	new_node->prev = curr;
	return (node);
}

void	flush_buf_if_needed(t_token **curr, char *buf, int *buf_index)
{
	if (*buf_index > 0)
	{
		buf[*buf_index] = '\0';
		*curr = add_token(*curr, buf, TOKEN_WORD);
		*buf_index = 0;
	}
}

int	is_quotes_closed(const char *start)
{
	char	quote;
	int		i;

	quote = *start;
	i = 1;
	while (start[i])
	{
		if (start[i] == quote)
			return 1;
		i++;
	}
	return 0;
}

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
