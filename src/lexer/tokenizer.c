
// #include "../../include/minishell.h"

// typedef enum e_token_type
// {
// 	TOKEN_WORD,
// 	TOKEN_PIPE,
// 	TOKEN_REDIRECT_IN,
// 	TOKEN_REDIRECT_OUT,
// 	TOKEN_REDIRECT_APPEND,
// 	TOKEN_HEREDOC,
// 	TOKEN_SPACE,
// 	TOKEN_COMMAND,
// 	TOKEN_ARGUMENT,
// 	TOKEN_FIELD,
// 	TOKEN_EXP_FIELD,
// 	TOKEN_VAR,
// 	TOKEN_FILE,
// 	TOKEN_HEREDOC_MARKER
// }							t_token_type;

// typedef struct s_token
// {
// 	char					*str;
// 	int						type;
// 	struct s_token			*next;
// 	struct s_token			*prev;
// }							t_token;

// static t_token *add_token(t_token *node, char *str, int type)
// {
//     t_token *new_node;
//     t_token *curr;

//     new_node = create_token_node(str, type);
//     if (!new_node)
//         return (NULL);
//     if (!node)
//         return (new_node);
//     curr = node;
//     while (curr->next)
//         curr = curr->next;
//     curr->next = new_node;
//     new_node->prev = curr;
//     return (node);
// }

// void print_tokens(t_token *tokens)
// {
//     while (tokens)
//     {
//         printf("Token: %s, Type: %d\n", tokens->str, tokens->type);
//         tokens = tokens->next;
//     }
// }

// static t_token *create_token_node(char *str, int type)
// {
//     t_token *new_node;

//     new_node = (t_token *)malloc(sizeof(t_token));
//     if (!new_node)
//         return (NULL);
//     new_node->str = ft_strdup(str);
//     new_node->type = type;
//     new_node->next = NULL;
//     new_node->prev = NULL;
//     return (new_node);
// }



// t_token *tokenize(char *s)
// {
// 	t_token *curr = NULL;
// 	int i = 0;
// 	char buf[256];
// 	char variable_buffer[256];
// 	int buf_index = 0;
// 	int var_index = 0;

// 	while (s[i] != '\0')
// 	{
// 		if (s[i] == '|' || s[i] == '<' || s[i] == '>' || s[i] == ' ' || (s[i] >= 9 && s[i] <= 13))
// 		{
// 			if (buf_index > 0)
// 			{
// 				buf[buf_index] = '\0';
// 				curr = add_token(curr, buf, TOKEN_WORD);
// 				buf_index = 0;
// 			}
// 			curr = add_operator_token(curr, s[i], s[i + 1], &i);
// 			i++;
// 		}
// 		else if (s[i] == '\'' || s[i] == '"')
// 		{
// 			if (buf_index > 0)
// 			{
// 				buf[buf_index] = '\0';
// 				curr = add_token(curr, buf, "WORD");
// 				buf_index = 0;
// 			}
// 			if (is_quotes_closed(&s[i]))
// 			{
// 				int len = extract_field(&s[i], curr);
// 				i += (len);
// 			}
// 			else
// 			{
// 				printf("quotes are not closed, syntax error\n");
// 				exit(1);
// 			}
// 		}
// 		else if(s[i] == '$')
// 		{
// 			if (buf_index > 0)
// 			{
// 				buf[buf_index] = '\0';
// 				curr = add_token(curr, buf, "WORD");
// 				buf_index = 0;
// 			}
// 			var_index = 0;
// 			i++;
// 			if ((s[i] >= 'a' && s[i] <= 'z') || (s[i] >= 'A' && s[i] <= 'Z') || s[i] == '_')
// 			{
// 				while ((s[i] >= 'a' && s[i] <= 'z') || (s[i] >= 'A' && s[i] <= 'Z') ||
// 					   s[i] == '_' || (s[i] >= '0' && s[i] <= '9'))
// 				{
// 					variable_buffer[var_index++] = s[i++];
// 				}
// 				variable_buffer[var_index] = '\0';
// 				curr = add_token(curr, variable_buffer, "VAR");
// 			}
// 		}
// 		else
// 		{
// 			buf[buf_index++] = s[i++];
// 		}
// 	}
// 	if (buf_index > 0)
// 	{
// 		buf[buf_index] = '\0';
// 		curr = add_token(curr, buf, "WORD");
// 		buf_index = 0;
// 	}
// 	return curr;
// }


// int main()
// {
//     t_token *test;
//     char input[] = "echo Hello world > out.txt";

//     test = tokenize(input);



// }