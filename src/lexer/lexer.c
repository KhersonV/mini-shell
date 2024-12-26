
#include "../../include/minishell.h"

int	is_not_word(char *str, int i)
{
	if ((str[i] > 8 && str[i] < 14) || (str[i] == 32))
		return (TOKEN_SPACE);
	else if (str[i] == '<' && str[i + 1] == '<')
		return (TOKEN_HEREDOC);
	else if (str[i] == '>' && str[i + 1] == '>')
		return (TOKEN_REDIRECT_APPEND);
	else if (str[i] == '|')
		return (TOKEN_PIPE);
	else if (str[i] == '>')
		return (TOKEN_REDIRECT_OUT);
	else if (str[i] == '<')
		return (TOKEN_REDIRECT_IN);
	else
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
			return("TOKEN_ARGUMENT");
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
		default:
			return ("UNKNOWN_TYPE");
	}
}


// char	*print_token(int current_token)
// {
// 	if (current_token == TOKEN_SPACE)
// 		return ("separator");
// 	else if (current_token == TOKEN_HEREDOC)
// 		return ("heredoc");
// 	else if (current_token == TOKEN_REDIRECT_APPEND)
// 		return ("append");
// 	else if (current_token == TOKEN_PIPE)
// 		return ("pipe");
// 	else if (current_token == TOKEN_REDIRECT_OUT)
// 		return ("output");
// 	else if (current_token == TOKEN_REDIRECT_IN)
// 		return ("input");
// 	else if (current_token == END)
// 		return ("end of line");
// 	else
// 		return ("probabily the start of a word");
// }

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

int	is_quotes_closed(char *s)
{
	char	left_quote;
	int		i;

	i = 1;
	left_quote = *s;
	s++;
	while (s[i])
	{
		if (left_quote == s[i])
			return (i);
		i++;
	}
	return (0);
}

char	*dup_field(char *s, int len)
{
	char	*out;
	int		i;

	i = 0;
	out = malloc((sizeof(char) * len) + 1);
	while (i < len)
	{
		out[i] = s[i];
		i++;
	}
	out[i] = '\0';
	return (out);
}

int	extract_field(char *s, t_token *element)
{
	int		len;
	char	quote;
	char	*field;

	len = 0;
	quote = *s;
	s++;
	while (s[len] != quote && s[len] != '\0')
		len++;
	if (s[len] == '\0')
	{
		printf("Syntax error, unmatched quote\n");
		exit(1);
	}
	field = dup_field(s, len);
	if (quote == '"')
		add_token(element, field, TOKEN_EXP_FIELD);
	else if (quote == '\'')
		add_token(element, field, TOKEN_FIELD);
	return (len + 2);
}
t_token *add_operator_token(t_token *curr, char current_char, char next_char, int *i)
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
	else if (current_char == ' ' || (current_char >= 9 && current_char <= 13))
		curr = add_token(curr, "[]", TOKEN_SPACE);
	return curr;
}

t_token *tokenize(char *s)
{
	t_token *curr = NULL;
	int i = 0;
	char buf[256];
	char variable_buffer[256];
	int buf_index = 0;
	int var_index = 0;

	while (s[i] != '\0')
	{
		if (s[i] == '|' || s[i] == '<' || s[i] == '>' || s[i] == ' ' || (s[i] >= 9 && s[i] <= 13))
		{
			if (buf_index > 0)
			{
				buf[buf_index] = '\0';
				curr = add_token(curr, buf, TOKEN_WORD);
				buf_index = 0;
			}
			curr = add_operator_token(curr, s[i], s[i + 1], &i);
			i++;
		}
		else if (s[i] == '\'' || s[i] == '"')
		{
			if (buf_index > 0)
			{
				buf[buf_index] = '\0';
				curr = add_token(curr, buf, TOKEN_WORD);
				buf_index = 0;
			}
			if (is_quotes_closed(&s[i]))
			{
				int len = extract_field(&s[i], curr);
				i += (len);
			}
			else
			{
				printf("quotes are not closed, syntax error\n");
				exit(1);
			}
		}
		else if(s[i] == '$')
		{
			if (buf_index > 0)
			{
				buf[buf_index] = '\0';
				curr = add_token(curr, buf, TOKEN_WORD);
				buf_index = 0;
			}
			var_index = 0;
			i++;
			if ((s[i] >= 'a' && s[i] <= 'z') || (s[i] >= 'A' && s[i] <= 'Z') || s[i] == '_')
			{
				while ((s[i] >= 'a' && s[i] <= 'z') || (s[i] >= 'A' && s[i] <= 'Z') ||
					   s[i] == '_' || (s[i] >= '0' && s[i] <= '9'))
				{
					variable_buffer[var_index++] = s[i++];
				}
				variable_buffer[var_index] = '\0';
				curr = add_token(curr, variable_buffer, TOKEN_VAR);
			}
		}
		else
		{
			buf[buf_index++] = s[i++];
		}
	}
	if (buf_index > 0)
	{
		buf[buf_index] = '\0';
		curr = add_token(curr, buf, TOKEN_WORD);
		buf_index = 0;
	}
	return curr;

}

void	temp_print_tokens(t_token *node)
{
	t_token	*curr;

	curr = node;
	while (curr)
	{
		printf("Token: %s, Type: %s\n", curr->str, print_token(curr->type)); // Use print_token to convert type to string
		curr = curr->next;
	}
}



// char	*print_token(int current_token)
// {
// 	switch (current_token)
// 	{
// 		case TOKEN_SPACE:
// 			return ("TOKEN_SPACE");
// 		case TOKEN_HEREDOC:
// 			return ("TOKEN_HEREDOC");
// 		case TOKEN_REDIRECT_APPEND:
// 			return ("TOKEN_REDIRECT_APPEND");
// 		case TOKEN_PIPE:
// 			return ("TOKEN_PIPE");
// 		case TOKEN_REDIRECT_OUT:
// 			return ("TOKEN_REDIRECT_OUT");
// 		case TOKEN_REDIRECT_IN:
// 			return ("TOKEN_REDIRECT_IN");
// 		case TOKEN_WORD:
// 			return ("TOKEN_WORD");
// 		case TOKEN_VAR:
// 			return ("TOKEN_VAR");
// 		case TOKEN_EXP_FIELD:
// 			return ("TOKEN_EXP_FIELD");
// 		case TOKEN_FIELD:
// 			return ("TOKEN_FIELD");
// 		case TOKEN_HEREDOC_MARKER:
// 			return ("TOKEN_HEREDOC_MARKER");
// 		case END:
// 			return ("END");
// 		default:
// 			return ("UNKNOWN_TYPE");
// 	}
// }

void remove_space_tokens(t_token **tree)
{
    t_token *curr = *tree;
    t_token *temp;

    while (curr != NULL)
    {
        if (curr->type == TOKEN_SPACE)
        {
            // Remove the space token
            if (curr->prev)
                curr->prev->next = curr->next;
            if (curr->next)
                curr->next->prev = curr->prev;

            // Update head if needed
            if (curr == *tree)
                *tree = curr->next;

            // Free the current node
            temp = curr;
            curr = curr->next;
            free(temp->str);
            free(temp);
        }
        else
        {
            curr = curr->next;
        }
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

char	*expand()
{
	char variable[] = "var_value";

	char *ptr = &variable;
	return ptr;
}

int	is_var_inside(char *s)
{
	while(*s)
	{
		if(*s == '$')
			return 1;
		s++;
	}
	return 0;
}

void expand_in_field()
{
	char *variable;

	variable = expand();
}

void expansion(t_token **tokens)
{
	t_token *curr;

	curr = *tokens;

	while(curr != NULL)
	{
		if(curr->type == TOKEN_EXP_FIELD)
		{
			if(is_var_inside(curr->str))
			{
				expand_in_field();
			}
		} else if (curr->type == TOKEN_VAR)
		{
			expand();
		}

		curr = curr->next;
	}
}


int main()
{
	t_token *test;
    // char input[] = "echo Hello world > out.txt | grep 'pattern' < in.txt";
	// char input[] = "echo 'static text' \"$DYNAMIC_VAR\" $USER";
	// char input[] = "echo Hello | grep 'pattern' > out.txt";
	char input[] = "cat << EOF | echo \"$HOME\"";

    printf("Input command: %s\n", input);
    test = tokenize(input);

    printf("\nTokens:\n");
    temp_print_tokens(test);

	remove_space_tokens(&test);

	expansion(&test);

	printf("\nTokens:\n");
    temp_print_tokens(test);

	adjusting_token_tree(&test);

	printf("\nTokens after adjustment:\n");
    temp_print_tokens(test);


    return 0;
}