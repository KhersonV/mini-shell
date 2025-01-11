
#include "../../include/minishell.h"

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
					return (i);
				i++;
			}
			else
			{
				if (append_char_to_buf(buf, buf_index, buf_size, '\\') < 0)
					return (i);
				if (append_char_to_buf(buf, buf_index, buf_size, input[i]) < 0)
					return (i);
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
				return (i);
			i++;
		}
	}
	if (input[i] == '"')
		i++;
	return (i);
}

static int	handle_dollar(const char *input, int *i, char *buf, int *buf_index,
		int buf_size, t_info *info)
{
	append_expanded_unquoted(input, i, buf, buf_index, buf_size, info);
	return (0);
}

static int	handle_escape(const char *input, int *i, char *buf, int *buf_index,
		int buf_size)
{
	(*i)++;
	if (!input[*i])
		return (-1);
	if (strchr("$\\\"'", input[*i]))
	{
		if (append_char_to_buf(buf, buf_index, buf_size, input[*i]) < 0)
			return (-1);
	}
	else
	{
		if (append_char_to_buf(buf, buf_index, buf_size, '\'') < 0)
			return (-1);
		if (append_char_to_buf(buf, buf_index, buf_size, input[*i]) < 0)
			return (-1);
	}
	(*i)++;
	return (0);
}

static int	handle_default(const char *input, int *i, char *buf, int *buf_index,
		int buf_size)
{
	if (append_char_to_buf(buf, buf_index, buf_size, input[*i]) < 0)
		return (-1);
	(*i)++;
	return (0);
}

static int	should_break(char c)
{
	return (is_space_char(c) || is_operator_char(c) || c == '\'' || c == '"');
}

static int	read_unquoted(const char *input, char *buf, int *buf_index,
		int buf_size, t_info *info)
{
	int	i;

	i = 0;
	while (input[i] != '\0')
	{
		if (should_break(input[i]))
			break ;
		if (input[i] == '$')
		{
			if (handle_dollar(input, &i, buf, buf_index, buf_size, info) < 0)
				return (i);
			continue ;
		}
		if (input[i] == '\\')
		{
			if (handle_escape(input, &i, buf, buf_index, buf_size) < 0)
				return (i);
			continue ;
		}
		if (handle_default(input, &i, buf, buf_index, buf_size) < 0)
			return (i);
	}
	return (i);
}

static void	skip_spaces(t_lexer_params *params)
{
	while (is_space_char(params->input[*params->i]))
	{
		flush_buf_if_needed(params->head, params->buf, params->buf_index);
		(*params->i)++;
	}
}

static int	handle_double_quote(t_token **head, char *buf, int *buf_index,
		const char *user_input, int *i, t_info *info)
{
	int		old_index;
	int		consumed;
	char	c;

	if (user_input[*i] != '"')
		return (0);
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
	return (1);
}

static int	handle_operator_char(t_token **head, char *buf, int *buf_index,
		const char *user_input, int *i)
{
	if (!is_operator_char(user_input[*i]))
		return (0);
	flush_buf_if_needed(head, buf, buf_index);
	*head = add_operator_token(*head, user_input[*i], user_input[*i + 1], i);
	*i = *i + 1;
	return (1);
}

static int	handle_single_quote(t_token **head, char *buf, int *buf_index,
		const char *user_input, int *i)
{
	int		old_index;
	int		consumed;
	char	c;

	if (user_input[*i] != '\'')
		return (0);
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
	return (1);
}

static int	handle_unquoted(char *buf, int *buf_index, const char *user_input,
		int *i, t_info *info)
{
	int	consumed;

	if (is_space_char(user_input[*i]) || is_operator_char(user_input[*i])
		|| user_input[*i] == '\'' || user_input[*i] == '"')
	{
		return (0);
	}
	consumed = read_unquoted(&user_input[*i], buf, buf_index, 1024, info);
	*i = *i + consumed;
	return (1);
}

int	process_token(t_token **head, char *buf, int *buf_index,
		const char *user_input, int *i, t_info *info)
{
	if (handle_operator_char(head, buf, buf_index, user_input, i))
		return (1);
	if (handle_single_quote(head, buf, buf_index, user_input, i))
		return (1);
	if (handle_double_quote(head, buf, buf_index, user_input, i, info))
		return (1);
	if (handle_unquoted(buf, buf_index, user_input, i, info))
		return (1);
	return (0);
}

t_token	*tokenizer(char *user_input, t_info *info)
{
	t_lexer_params	params;
	t_token			*head;
	char			buf[1024];
	int				buf_index;
	int				i;

	head = NULL;
	buf_index = 0;
	i = 0;
	params.head = &head;
	params.buf = buf;
	params.buf_index = &buf_index;
	params.input = user_input;
	params.i = &i;
	params.info = info;
	params.buf_size = 1024;

	while (user_input[i] != '\0')
	{
		skip_spaces(&params);
		if (process_token(&head, buf, &buf_index, user_input, &i, info))
			continue ;
		break ;
	}
	flush_buf_if_needed(&head, buf, &buf_index);
	return head;
}
