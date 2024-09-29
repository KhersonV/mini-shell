/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: snazarov <snazarov@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/22 21:23:27 by lynchsama         #+#    #+#             */
/*   Updated: 2024/09/29 14:23:14 by snazarov         ###   ########.fr       */
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
	SPACE = 1,
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
		return SPACE;
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
	if(current_token == SPACE)
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

int is_quotes_closed(char *s)
{
	char left_quote;
	int i;
	
	left_quote = *s;
	s++;
	while(s[i])
	{
		if(left_quote = s[i])
			return i;
		i++;
	}
		return 0;
}

char *dup_string(char *s, int start, int end, int len)
{

}

int extract_field(char *s, int *i, t_tree *element)
{
	int type;
	char quote = *s;
	if(*s == '\'')
		type = 1;
	else if(*s == '"')
		type = 2;
	
	*(i++);
	int start = *i;
	while(s[*i] != quote)
		*(i++);
	int end = *i;
	

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
		if(s[i] == '|' || s[i] == '<' || s[i] == '>' || s[i] == ' ')
		{
			if(buf_index > 0)
			{
			buf[buf_index] = '\0';
			curr = add_token(curr, buf, "WORD");
			buf_index = 0;
			}
			if(s[i] == '|')
			{
				curr = add_token(curr, "|", "PIPE");
			} else if(s[i] == '<')
			{
				if(s[i + 1] == '<')
				{
					curr = add_token(curr, "<<", "REDIR_INSOURCE");
					i++;
				} else
				{
					curr = add_token(curr, "<", "REDIR_IN");
				}
			} else if(s[i] == '>')
			{
				if(s[i + 1] == '>')
				{
					curr = add_token(curr, ">>", "REDIR_APPEND");
					i++;
				} else {
					curr = add_token(curr, ">", "REDIR_OUT");
				}
			} else if(s[i] == ' ')
			{
				curr = add_token(curr, "[]", "SPACE");
			}
			/*field condition*/
		} else if(s[i] == '\'' || s[i] == '"')
		{
			if(is_quotes_closed(s[i]))
			{
				extract_field(s[i], &i, curr);
			} else
			{
				printf("quotes are not closed, syntax error");
			}
		} else 
		{
			buf[buf_index++] = s[i];
		}
		i++;
	}
	if(buf_index > 0){
		buf[buf_index] = '\0';
		curr = add_token(curr, buf, "WORD");
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