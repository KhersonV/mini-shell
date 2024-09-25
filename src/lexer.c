/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lynchsama <lynchsama@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/22 21:23:27 by lynchsama         #+#    #+#             */
/*   Updated: 2024/09/25 20:13:18 by lynchsama        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct s_tree
{
	char 			*type;
	char			*name;
	struct s_tree	*next;
	struct s_tree	*right;
}					t_tree;

typedef struct s_token
{
	char	*str;
	int		type;
	struct s_token *next;
	struct s_token *prev;
}	t_token;

enum token_types {
	SEP = 1,
	WORD,
	FIELD,
	EXP_FIELD,
	REDIR_OUT,
	REDIR_IN,
	REDIR_APPEND,
	REDIR_INSOURCE,
	PIPE,
	END
};

int is_not_word(char *str, int i)
{
	if((str[i]  > 8 && str[i] < 14) || (str[i] == 32))
		return SEP;
	else if(str[i] == '<' && str[i+1] == '<')
		return REDIR_INSOURCE;
	else if(str[i] == '>' && str[i + 1] == '>')
		return REDIR_APPEND;
	else if(str[i] == '|')
		return PIPE;
	else if(str[i] == '>')
		return REDIR_OUT;
	else if(str[i] == '<')
		return REDIR_IN;
	else if(str[i] == '\0')
		return END;
	else
		return 0;

}

char  *print_token(enum token_types current_token)
{
	if(current_token == SEP)
		return ("separator");
	else if(current_token == REDIR_INSOURCE)
		return ("heredoc");
	else if(current_token == REDIR_APPEND)
		return ("append");
	else if(current_token == PIPE)
		return ("pipe");
	else if(current_token == REDIR_OUT)
		return ("output");
	else if(current_token == REDIR_IN)
		return ("input");
	else if(current_token == END)
		return ("end of line");
	else
		return ("probabily the start of a word");
}

t_tree *create_node(char *name, char *type)
{
	t_tree *new_node = (t_tree *)malloc(sizeof(t_tree));
	if(!new_node)
		return (NULL);
	new_node->name = strdup(name);
	new_node->type = strdup(type);
	new_node->next = NULL;
	return new_node;
}

t_tree *add_token(t_tree *node, char *name, char *type)
{
	t_tree *new_node = create_node(name, type);
	if(!new_node)
		return NULL;
	if(!node)
		return new_node;

	t_tree *curr = node;
	while(curr->next)
	{
		curr = curr->next;
	}
	curr->next = new_node;
	return node;
}


t_tree *tokenize(char *s)
{
	t_tree *curr;
	int i;
	char buf[256];
	int buf_index;
	int token_type;

	curr = NULL;
	buf_index = 0;

	while(s[i] != '\0')
	{
		printf("%c\n", s[i]);
		if(s[i] == '|' || s[i] == '<' || s[i] == '>')
		{
			buf[buf_index] = '\0';
			curr = add_token(curr, buf, "WORD");
			buf_index = 0;
		} else
		{
			buf[buf_index++] = s[i];
		}
		i++;
	}
	return curr;
}

void print_tokens(t_tree *node)
{
	t_tree *curr = node;
	while(curr)
	{
		printf("Token: %s, Type: %s\n", curr->name, curr->type);
		curr = curr->next;
	}
}

int main(int argc, char **argv)
{
	char input[] = "echo hello >> file.txt | cat << input.txt";

	t_tree *root;
	root = tokenize(input);
	print_tokens(root);


}