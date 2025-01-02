
#include "../../include/minishell.h"

char *expand_variable(char *var_name, t_info *info);

char *ft_expand_variable(char *var_name, t_info *info)
{
    // if (ft_strcmp(var_name, "?") == 0)
    // {
    //     char *exit_str = ft_itoa(info->exit_status);
    //     return exit_str;
    // }
    // else if (ft_strcmp(var_name, "$") == 0)
    // {
    //     char *pid_str = ft_itoa(getpid());
    //     return pid_str;
    // }
    // else
    // {
    char *val = get_env_value(info, var_name);
    if (val == NULL)
        return ft_strdup("");
    return ft_strdup(val);
    // }
}




// TODO : echo $$HOME,  $$ check.
void expansion(t_token **tokens, t_info *info);

static int is_space_char(char c)
{
	return (c == ' ' || (c >= 9 && c <= 13));
}

static int is_operator_char(char c)
{
	return (c == '|' || c == '<' || c == '>');
}

int	ft_isalnum(int c)
{
	if ((c >= '0' && c <= '9')
		|| ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')))
		return (1);
	return (0);
}


// static int is_special_char(char c)
// {
// 	if (is_space_char(c))        return 1;
// 	if (is_operator_char(c))     return 1;
// 	if (c == '$')                return 1;
// 	if (c == '\'' || c == '"')   return 1;
// 	return 0;
// }

static char* read_var_name(const char *input, int *consumed)
{
    int i = 0;
    if (input[i] == '$') {
        i++;
        if (input[i] == '?') {
            i++;
            *consumed = i;
            return strdup("?");
        }

        char var_buf[256];
        int var_idx = 0;
        while (input[i] && (isalnum((unsigned char)input[i]) || input[i] == '_')) {
            if (var_idx < 255) {
                var_buf[var_idx++] = input[i];
            }
            i++;
        }
        var_buf[var_idx] = '\0';
        *consumed = i; 
        if (var_idx == 0) { 
            return strdup("$"); 
        }
        return strdup(var_buf);
    }
    *consumed = 0;
    return NULL;
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
		case TOKEN_EXIT_STATUS:
			return ("TOKEN_EXIT_STATUS");
		default:
			return ("UNKNOWN_TYPE");
	}
}

static int append_char_to_buf(char *buf, int *idx, int buf_size, char c)
{
    if (*idx >= buf_size - 1) {
        return -1; 
    }
    buf[(*idx)++] = c;
    return 0;
}

static int read_single_quoted(const char *input, char *buf, int *buf_index, int buf_size)
{
    int i = 1; 
    while (input[i] && input[i] != '\'') {
        if (append_char_to_buf(buf, buf_index, buf_size, input[i]) < 0) {
            fprintf(stderr, "Buffer overflow in single quotes\n");
            return i;
        }
        i++;
    }
    if (input[i] == '\'') {
        i++;
    }
    return i; 
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


static char* expand_dollar(const char *input, int *consumed, t_info *info)
{
    int var_consumed = 0;
    char *var_name = read_var_name(input, &var_consumed);
    if (!var_name) {
        *consumed = 0;
        return strdup("");
    }

	*consumed = var_consumed;

	if (strcmp(var_name, "$") == 0) 
	{
        // free(var_name);
        return strdup("$");
    }

	if (strcmp(var_name, "?") == 0) {
        // free(var_name);
        char *exit_str = ft_itoa(info->exit_status);
        return exit_str;
    }

	char *expanded = ft_expand_variable(var_name, info);
    // free(var_name);
    return expanded; // уже malloc'нута

	return var_name;
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
	if(*buf_index > 0)
	{
		buf[*buf_index] = '\0';
		*curr = add_token(*curr, buf, TOKEN_WORD);
		*buf_index = 0;
	}
}

int is_quotes_closed(const char *start)
{
	char quote = *start;
	int i = 1;
	while (start[i]) {
		if (start[i] == quote)
			return 1;
		i++;
	}
	return 0;
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
	// else if (current_char == ' ' || (current_char >= 9 && current_char <= 13))
	// 	curr = add_token(curr, "[]", TOKEN_SPACE);
	return curr;
}

// void handle_variable(t_token **p_head, const char *s, int *i)
// {
// 	(*i)++;
// 	if (!s[*i]) {
// 		*p_head = add_token(*p_head, "$", TOKEN_WORD);
// 		return;
// 	}
// 	if (s[*i] == '?') {
// 		(*i)++;
// 		*p_head = add_token(*p_head, "$?", TOKEN_EXIT_STATUS);
// 		return;
// 	}
// 	if (ft_isalnum(s[*i]) || s[*i] == '_') {
// 		char var_buf[256];
// 		int vindex = 0;

// 		var_buf[vindex++] = '$';

// 		while (s[*i] != '\0' && !is_space_char(s[*i])) {
// 			var_buf[vindex++] = s[*i];
// 			(*i)++;
// 			if (vindex >= 255) break;
// 		}
// 		var_buf[vindex] = '\0';

// 		*p_head = add_token(*p_head, var_buf, TOKEN_VAR);
// 	}
// 	else {
// 		*p_head = add_token(*p_head, "$", TOKEN_WORD);
// 	}
// }

// int handle_quotes(t_token **p_head, const char *s, int *i)
// {
// 	char quote = s[*i];
// 	int start = *i + 1;
// 	int len = 0;

// 	int j = start;
// 	while (s[j] && s[j] != quote) {
// 		j++;
// 	}
// 	if (!s[j]) {
// 		printf("Syntax error: quotes not closed\n");
// 		exit(1);
// 	}

// 	len = j - start;
// 	char *field = malloc(len + 1);
// 	if (!field) { /* ... */ }
// 	strncpy(field, &s[start], len);
// 	field[len] = '\0';

// 	if(strcmp(field, ""))
// 	{
// 	if (quote == '"')
// 		*p_head = add_token(*p_head, field, TOKEN_EXP_FIELD);
// 	else
// 		*p_head = add_token(*p_head, field, TOKEN_FIELD);
// 	}

// 	// free(field);

// 	*i = j + 1;

// 	return 1;
// }

// void handle_special_char(t_token **p_head, const char *s, int *i)
// {
// 	if (is_space_char(s[*i])) {
// 		// *p_head = add_operator_token(*p_head, s[*i], s[*i + 1], i);
// 		(*i)++;
// 		return;
// 	}
// 	if (is_operator_char(s[*i])) {
// 		*p_head = add_operator_token(*p_head, s[*i], s[*i + 1], i);
// 		(*i)++;
// 		return;
// 	}
// 	if (s[*i] == '\'' || s[*i] == '"') {
// 		if (!is_quotes_closed(&s[*i])) {
// 			printf("Quotes not closed\n");
// 			exit(1);
// 		}
// 		handle_quotes(p_head, s, i);
// 		return;
// 	}
// 	if (s[*i] == '$') {
// 		handle_variable(p_head, s, i);
// 		return;
// 	}
// }

static int read_double_quoted(const char *input, char *buf, int *buf_index, int buf_size, t_info *info)
{
	int i = 1;
	while(input[i] && input[i] != '"')
	{
		if (input[i] == '\\')
		{
			i++;
			if(!input[i])
			{
				break;
			}
			if(input[i] == '"' || input[i] == '$' || input[i] == '\\')
			{
				if(append_char_to_buf(buf, buf_index, buf_size, input[i]) < 0) 
				{
					fprintf(stderr, "Buffer overflow in double quotes\n");
                    return i;
				}
				i++;
			} else {
                if (append_char_to_buf(buf, buf_index, buf_size, '\\') < 0) {
                    fprintf(stderr, "Buffer overflow in double quotes\n");
                    return i;
                }
                if (append_char_to_buf(buf, buf_index, buf_size, input[i]) < 0) {
                    fprintf(stderr, "Buffer overflow in double quotes\n");
                    return i;
                }
                i++;
			}
		} else if (input[i] == '$') 
		{
			int consumed = 0;
			char *expanded = expand_dollar(&input[i], &consumed, info);


			for(int k = 0; expanded[k] != '\0'; k++)
			{
				if (append_char_to_buf(buf, buf_index, buf_size, expanded[k]) < 0) {
                    fprintf(stderr, "Buffer overflow in double quotes\n");
                    // free(expanded);
                    return i + consumed;
                }
				// printf("buffer - %s\n", buf);
			}
			// free(expanded);
			i += consumed;
		} 
		else
		{
            if (append_char_to_buf(buf, buf_index, buf_size, input[i]) < 0) {
                fprintf(stderr, "Buffer overflow in double quotes\n");
                return i;
            }
            i++;
        }
    }
	// printf("last char in field - [%c]\n", input[i]); - []
    if (input[i] == '"') {
        i++; 
    }
    return i;
}

static int read_unquoted(const char *input, char *buf, int *buf_index, int buf_size, t_info *info)
{
    int i = 0;
    while (input[i] != '\0') {
        if (is_space_char(input[i]) || is_operator_char(input[i])) {
            break;
        }
        if (input[i] == '\'' || input[i] == '"') {
            break;
        }
        if (input[i] == '$') {
            int consumed = 0;
            char *expanded = expand_dollar(&input[i], &consumed, info);
            for (int k = 0; expanded[k] != '\0'; k++) {
                if (append_char_to_buf(buf, buf_index, buf_size, expanded[k]) < 0) {
                    fprintf(stderr, "Buffer overflow in unquoted\n");
                    // free(expanded);
                    return i + consumed;
                }
            }
            // free(expanded);
            i += consumed;
            continue;
        }
        if (input[i] == '\\') {
            i++;
            if (!input[i]) break;

            if (strchr("$\\\"\'", input[i])) {
                if (append_char_to_buf(buf, buf_index, buf_size, input[i]) < 0) {
                    fprintf(stderr, "Buffer overflow in unquoted (backslash)\n");
                    return i;
                }
                i++;
            } else {
                // Иначе пишем сам бэкслэш + символ
                if (append_char_to_buf(buf, buf_index, buf_size, '\\') < 0) {
                    fprintf(stderr, "Buffer overflow in unquoted (backslash)\n");
                    return i;
                }
                // второй символ
                if (append_char_to_buf(buf, buf_index, buf_size, input[i]) < 0) {
                    fprintf(stderr, "Buffer overflow in unquoted (backslash)\n");
                    return i;
                }
                i++;
            }
            continue;
        }

        if (append_char_to_buf(buf, buf_index, buf_size, input[i]) < 0) {
            fprintf(stderr, "Buffer overflow in unquoted\n");
            return i;
        }
        i++;
    }
    return i;
}



t_token *tokenizer(char *user_input, t_info *info)
{
	t_token *head = NULL;

	char buf[1024];
	int buf_index = 0;

	int i = 0;

	while(user_input[i] != '\0')
	{
		// printf("next char - [%c]\n", user_input[i]);
		if(is_space_char(user_input[i]))
		{
			flush_buf_if_needed(&head, buf, &buf_index);
			i++;
			continue;
		}
		if(is_operator_char(user_input[i]))
		{
			flush_buf_if_needed(&head, buf, &buf_index);
			head = add_operator_token(head, user_input[i], user_input[i+1], &i);
			i++;
			continue;
		}
		if(user_input[i] == '\'')
		{
			int consumed = read_single_quoted(&user_input[i], buf, &buf_index, 1024);
			i += consumed;
			
			continue;
		}
		if(user_input[i] == '\"')
		{
			int consumed = read_double_quoted(&user_input[i], buf, &buf_index, 1024, info);
			i += consumed;
			continue;
		}
		
		int consumed = read_unquoted(&user_input[i], buf, &buf_index, 1024, info);
		
        i += consumed;
	}

	flush_buf_if_needed(&head, buf, &buf_index);

	// printf("final buf = %s", buf);
	return head;
}

// t_token *tokenize(char *s)
// {
// 	t_token *head = NULL;
// 	char buf[256];
// 	int buf_index = 0;
// 	int i = 0;

// 	while (s[i] != '\0')
// 	{
// 		if (is_special_char(s[i]))
// 		{
// 			flush_buf_if_needed(&head, buf, &buf_index);
// 			handle_special_char(&head, s, &i);
// 		}
// 		else
// 		{
// 			buf[buf_index++] = s[i++];
// 			if (buf_index >= 255)
// 			{
// 				buf[buf_index] = '\0';
// 				head = add_token(head, buf, TOKEN_WORD);
// 				buf_index = 0;
// 			}
// 		}
// 	}
// 	flush_buf_if_needed(&head, buf, &buf_index);
// 	return head;
// }

// void	temp_print_tokens(t_token *node)
// {
// 	t_token	*curr;

// 	curr = node;
// 	while (curr)
// 	{
// 		printf("Token: %s, Type: %s\n", curr->str, print_token(curr->type));
// 		curr = curr->next;
// 	}
// }


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
		if (!command_found && (curr->type == TOKEN_WORD || curr->type == TOKEN_EXP_FIELD || curr->type == TOKEN_FIELD || curr->type == TOKEN_VAR))
		{
			curr->type = TOKEN_COMMAND;
			command_found = 1;
		}
		else if (command_found && (curr->type == TOKEN_WORD || curr->type == TOKEN_FIELD ||
								   curr->type == TOKEN_EXP_FIELD || curr->type == TOKEN_VAR
								   || curr->type == TOKEN_EXIT_STATUS))
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

// int	is_var_inside(char *s)
// {
// 	while(*s)
// 	{
// 		if(*s == '$')
// 			return 1;
// 		s++;
// 	}
// 	return 0;
// }

// char *expand_variable(const char *var_name)
// {
// 	char *value;

// 	value = getenv(var_name);

// 	if (!value)
// 		return "";
// 	return value;
// }


// char *expand_field(const char *str)
// {
// 	char buffer[1024];
// 	char var_name[256];
// 	char *var_value;
// 	const char *ptr;
// 	unsigned long buf_index;
// 	int var_index;
// 	int len;

// 	ptr = str;
// 	buf_index = 0;

// 	while (*ptr)
// 	{
// 		if (*ptr == '$')
// 		{
// 			ptr++;
// 			var_index = 0;

// 			while ((*ptr >= 'a' && *ptr <= 'z') || (*ptr >= 'A' && *ptr <= 'Z') ||
// 				   (*ptr == '_') || (*ptr >= '0' && *ptr <= '9'))
// 			{
// 				var_name[var_index] = *ptr;
// 				var_index++;
// 				ptr++;
// 			}
// 			var_name[var_index] = '\0';

// 			var_value = expand_variable(var_name);
// 			len = strlen(var_value);

// 			if (buf_index + len >= sizeof(buffer))
// 			{
// 				printf("Error: expansion buffer overflow\n");
// 				exit(1);
// 			}

// 			strcpy(&buffer[buf_index], var_value);
// 			buf_index += len;
// 		}
// 		else
// 		{
// 			buffer[buf_index] = *ptr;
// 			buf_index++;
// 			ptr++;
// 		}

// 		if (buf_index >= sizeof(buffer))
// 		{
// 			printf("Error: expansion buffer overflow\n");
// 			exit(1);
// 		}
// 	}

// 	buffer[buf_index] = '\0';
// 	return strdup(buffer);
// }

// void expand_in_field(t_token *token)
// {
// 	char *expanded;

// 	expanded = expand_field(token->str);
// 	free(token->str);
// 	token->str = expanded;
// }

// void expansion(t_token **tokens)
// {
// 	t_token *curr;
// 	char *expanded;

// 	curr = *tokens;

// 	while (curr != NULL)
// 	{
// 		if (curr->type == TOKEN_EXP_FIELD || curr->type == TOKEN_WORD)
// 		{
// 			if (is_var_inside(curr->str))
// 			{
// 				expand_in_field(curr);
// 			}
// 		}
// 		else if (curr->type == TOKEN_VAR)
// 		{
// 			expanded = expand_variable(curr->str);
// 			free(curr->str);
// 			curr->str = strdup(expanded);
// 		}

// 		curr = curr->next;
// 	}
// }


// int main()
// {
// 	t_token *test;
// 	t_token *test2;
// 	t_info *info;
// 	// char input[] = "echo Hello world > out.txt | grep 'pattern' < in.txt";
// 	// char input[] = "echo 'static text' \"$DYNAMIC_VAR\" $USER";
// 	// char input[] = "echo Hello | grep 'pattern' > out.txt";
// 	// char input[] = "cat $HOME.txt | echo \"$HOMEsomeworkds\" ";
// 	char input[] = "echo $HOME$? >> out.txt | echo $?\"42\"";

// 	// char input[] = "env VAR=HELLO";

// 	printf("Input command: %s\n", input);
// 	test = tokenize(input);

// 	test2 = tokenizer(input, info);

// 	printf("\nNew tokenizer:\n");
// 	temp_print_tokens(test2);
// 	printf("---------\n");


// 	// printf("\nTokens:\n");
// 	// temp_print_tokens(test);

// 	// remove_space_tokens(&test);

// 	// expansion(&test,info);

// 	// printf("\nTokens after expansion:\n");
// 	// temp_print_tokens(test);

// 	adjusting_token_tree(&test);

// 	printf("\nTokens after adjustment:\n");
// 	temp_print_tokens(test);

// 	return 0;
// }
