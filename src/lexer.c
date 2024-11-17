/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lynchsama <lynchsama@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/22 21:23:27 by lynchsama         #+#    #+#             */
/*   Updated: 2024/11/17 12:29:15 by lynchsama        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int	is_not_word(char *str, int i)
{
	if ((str[i] > 8 && str[i] < 14) || (str[i] == 32))
		return (TOKEN_SPACE);
	else if (str[i] == '<' && str[i + 1] == '<')
		return (REDIR_INSOURCE);
	else if (str[i] == '>' && str[i + 1] == '>')
		return (REDIR_APPEND);
	else if (str[i] == '|')
		return (PIPE);
	else if (str[i] == '>')
		return (REDIR_OUT);
	else if (str[i] == '<')
		return (REDIR_IN);
	else if (str[i] == '\0')
		return (END);
	else
		return (0);
}

char	*print_token(enum token_types current_token)
{
	if (current_token == TOKEN_SPACE)
		return ("separator");
	else if (current_token == REDIR_INSOURCE)
		return ("heredoc");
	else if (current_token == REDIR_APPEND)
		return ("append");
	else if (current_token == PIPE)
		return ("pipe");
	else if (current_token == REDIR_OUT)
		return ("output");
	else if (current_token == REDIR_IN)
		return ("input");
	else if (current_token == END)
		return ("end of line");
	else
		return ("probabily the start of a word");
}

t_tree	*create_token_node(char *name, char *type)
{
	t_tree	*new_node;

	new_node = (t_tree *)malloc(sizeof(t_tree));
	if (!new_node)
		return (NULL);
	new_node->name = ft_strdup(name);
	new_node->type = ft_strdup(type);
	new_node->next = NULL;
	new_node->prev = NULL;
	return (new_node);
}

t_tree	*add_token(t_tree *node, char *name, char *type)
{
	t_tree	*new_node;
	t_tree	*curr;

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

int	extract_field(char *s, t_tree *element)
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
		add_token(element, field, "EXP_FIELD");
	else if (quote == '\'')
		add_token(element, field, "FIELD");
	return (len + 2);
}
t_tree *add_operator_token(t_tree *curr, char current_char, char next_char, int *i)
{
	if (current_char == '|')
		curr = add_token(curr, "|", "PIPE");
	else if (current_char == '<')
	{
		if (next_char == '<')
		{
			curr = add_token(curr, "<<", "REDIR_INSOURCE");
			(*i)++;
		}
		else
			curr = add_token(curr, "<", "REDIR_IN");
	}
	else if (current_char == '>')
	{
		if (next_char == '>')
		{
			curr = add_token(curr, ">>", "REDIR_APPEND");
			(*i)++;
		}
		else
			curr = add_token(curr, ">", "REDIR_OUT");
	}
	else if (current_char == ' ')
		curr = add_token(curr, "[]", "TOKEN_SPACE");
	return curr;
}

t_tree *tokenize(char *s)
{
	t_tree *curr = NULL;
	int i = 0;
	char buf[256];
	char variable_buffer[256];
	int buf_index = 0;
	int var_index = 0;

	while (s[i] != '\0')
	{
		if (s[i] == '|' || s[i] == '<' || s[i] == '>' || s[i] == ' ')
		{
			if (buf_index > 0)
			{
				buf[buf_index] = '\0';
				curr = add_token(curr, buf, "WORD");
				buf_index = 0;
			}
			curr = add_operator_token(curr, s[i], s[i + 1], &i);
		}
		else if (s[i] == '\'' || s[i] == '"')
		{
			if (is_quotes_closed(&s[i]))
			{
				i += extract_field(&s[i], curr);
			}
			else
			{
				printf("quotes are not closed, syntax error\n");
				exit(1);
			}
		} else if(s[i] == '$')
		{
			var_index = 0;

			if ((s[i] >= 'a' && s[i] <= 'z') || (s[i] >= 'A' && s[i] <= 'Z') || s[i] == '_')
			{
				while ((s[i] >= 'a' && s[i] <= 'z') || (s[i] >= 'A' && s[i] <= 'Z') ||
			  		 s[i] == '_' || (s[i] >= '0' && s[i] <= '9'))
				{
					variable_buffer[var_index++] = s[i++];
				}
				variable_buffer[var_index] = '\0';
		curr = add_token(curr, variable_buffer, "VAR");
		}
		else
		{
			curr = add_token(curr, "", "VAR");
		}
		}
		else
		{
			buf[buf_index++] = s[i];
		}
		i++;
	}
	if (buf_index > 0)
	{
		buf[buf_index] = '\0';
		curr = add_token(curr, buf, "WORD");
	}

	return curr;
}

void	temp_print_tokens(t_tree *node)
{
	t_tree	*curr;

	curr = node;
	while (curr)
	{
		printf("Token: %s, Type: %s\n", curr->name, curr->type);
		curr = curr->next;
	}
}

// int main(int argc, char **argv)
// {
// 	// char input[] = "echo \'hello	>> file.txt | cat << input.txt";
// 	// //char input[] = "echo hello >> \"file.txt\" | cat << input.txt";

// 	// t_tree *root;
// 	// root = tokenize(input);
// 	// print_tokens(root);

// 	// char test[] = "test of \"the\" quotes";
// 	// char *ptr = &test;
// 	// while(*ptr)
// 	// {
// 	// 	if(*ptr == '\\')
// 	// 	{
// 	// 		printf("error");
// 	// 	}
// 	// 	printf("%c", *ptr);
// 	// 	ptr++;

// 	// }

// }