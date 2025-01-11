
#include "../../include/minishell.h"

char		*expand_dollar(const char *input, int *consumed, t_info *info);

char	*ft_expand_variable(char *var_name, t_info *info)
{
	char	*val;

	val = get_env_value(info, var_name);
	if (val == NULL)
		return (ft_strdup(""));
	return (ft_strdup(val));
}

int	is_space_char(char c)
{
	return (c == ' ' || (c >= 9 && c <= 13));
}

int	is_operator_char(char c)
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

int	append_char_to_buf(char *buf, int *idx, int buf_size, char c)
{
	if (*idx >= buf_size - 1)
	{
		return (-1);
	}
	buf[(*idx)++] = c;
	return (0);
}

int	read_single_quoted(const char *input, char *buf, int *buf_index,
		int buf_size)
{
	int	i;

	i = 1;
	while (input[i] && input[i] != '\'')
	{
		if (append_char_to_buf(buf, buf_index, buf_size, input[i]) < 0)
		{
			fprintf(stderr, "Buffer overflow in single quotes\n"); // todo
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

int	read_dollar_single(const char *input, char *buf, int *buf_index)
{
	int	i;
	int	stop;

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

int	handle_bslash_short(const char **p, char *buf, int *buf_index)
{
	int	stop;
	int	res;

	stop = 0;
	res = append_char_to_buf(buf, buf_index, 1024, (*p)[0]);
	if (res < 0)
		stop = 1;
	*p = (*p) + 1;
	return (stop);
}

int	handle_bslash_long(const char **p, char *buf, int *buf_index)
{
	int	local_stop;
	int	res;

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
	return (local_stop);
}

int	handle_dquotes_backslash(const char **p, char *buf, int *buf_index)
{
	int	stop;
	int	check;

	stop = 0;
	*p = (*p) + 1;
	if ((*p)[0] == '\0')
		return (stop);
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

int	handle_dquotes_dollar(const char **p, char *buf, int *buf_index,
		t_info *info)
{
	int		stop;
	int		var_consumed;
	char	*expanded;
	int		copy_stop;

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
			if (append_char_to_buf(buf, buf_index, 1024,
					expanded[copy_stop]) < 0)
				stop = 1;
			else
				copy_stop = copy_stop + 1;
		}
		free(expanded);
		*p = (*p) + var_consumed;
	}
	return (stop);
}

int	handle_dquotes_normal_char(const char **p, char *buf, int *buf_index)
{
	int	stop;
	int	res;

	stop = 0;
	res = append_char_to_buf(buf, buf_index, 1024, (*p)[0]);
	if (res < 0)
		stop = 1;
	else
		*p = (*p) + 1;
	return (stop);
}

int	read_dollar_double(const char *input, char *buf, int *buf_index,
		t_info *info)
{
	const char	*p;
	const char	*start;
	int			stop;

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

char	*read_dollar_quoted(const char *input, int *consumed, t_info *info)
{
	char	quote;
	char	buf[1024];
	int		buf_index;
	int		i;

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

int	is_early_termination(char next_char, char quote)
{
	return (next_char == '\0' || is_space_char(next_char)
		|| is_operator_char(next_char) || next_char == quote);
}

int	find_closing_quote(const char *input, char quote)
{
	int	j;

	j = 2;
	while (input[j])
	{
		if (input[j] == quote)
			return (j);
		j++;
	}
	return (-1);
}

char	*handle_no_closing_quote(int *consumed)
{
	*consumed = 1;
	return (ft_strdup("$"));
}

static char	*handle_special_variable(const char *var_name, t_info *info)
{
	if (ft_strcmp(var_name, "?") == 0)
	{
		return (ft_itoa(info->exit_status));
	}
	if (ft_strcmp(var_name, "$") == 0)
	{
		return (ft_strdup("$"));
	}
	return (NULL);
}

char	*handle_quoted_dollar(const char *input, int *consumed)
{
	char	quote;
	char	next_char;
	int		closing_quote_index;

	quote = input[1];
	next_char = input[2];
	if (is_early_termination(next_char, quote))
		return (handle_no_closing_quote(consumed));
	closing_quote_index = find_closing_quote(input, quote);
	if (closing_quote_index == -1)
		return (handle_no_closing_quote(consumed));
	return (NULL);
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
		return (strdup(""));
	}
	*consumed = var_consumed;
	expanded = handle_special_variable(var_name, info);
	if (expanded)
	{
		free(var_name);
		return (expanded);
	}
	expanded = ft_expand_variable(var_name, info);
	free(var_name);
	return (expanded);
}

char	*expand_dollar(const char *input, int *consumed, t_info *info)
{
	char	*result;

	if (input[1] == '\'' || input[1] == '"')
	{
		result = handle_quoted_dollar(input, consumed);
		if (result)
			return (result);
		return (read_dollar_quoted(input, consumed, info));
	}
	return (handle_variable_expansion(input, consumed, info));
}

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
			return (1);
		i++;
	}
	return (0);
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
	return (curr);
}
