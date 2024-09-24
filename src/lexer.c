/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lynchsama <lynchsama@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/22 21:23:27 by lynchsama         #+#    #+#             */
/*   Updated: 2024/09/24 21:16:22 by lynchsama        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>

typedef struct s_tree
{
	char 			*type;
	char			*name;
	struct s_tree	*left;
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

void print_token(enum token_types current_token)
{
	if(current_token == SEP)
		printf("separator");
	else if(current_token == REDIR_INSOURCE)
		printf("heredoc");
	else if(current_token == REDIR_APPEND)
		printf("append");
	else if(current_token == PIPE)
		printf("pipe");
	else if(current_token == REDIR_OUT)
		printf("output");
	else if(current_token == REDIR_IN)
		printf("input");
	else if(current_token == END)
		printf("end of line");
	else
		printf("probabily the start of a word");
}


void	looping_string(char *str)
{

	int start;
	int end;
	int i = 0;

	if(!(is_not_word(str, 0)))
	{
		start = 0;
		while(!((str[i]  > 8 && str[i] < 14) || (str[i] == 32) || str[i] == '\0'))
		{
			i++;
		}
		end = i;
		printf("start = %d, end = %d", start, end);
	}
}

void make_token(char *s, int start, int end)
{
	if(start != end)
	{
	while(start <= end)
	{

		printf("%c", s[start]);
		start++;
	}
	} else
	{
		if(s[start] == ' ')
			printf("[ ]");
		else
			printf("%c", s[start]);
	}
	printf("\n");
}

void lst_find_last_node(t_tree **root)
{

}

void add_token_node(char *s, int start, int end, t_tree **root)
{
	t_tree *node;

	node = malloc(sizeof(t_tree));

}

void loop(char *s, t_tree **root)
{
	int i = 0;
	int start;
	int end;
	enum token_types current_token;
	int in_word = 0;


	start = i;
	while(s[i] != '\0')
	{
		current_token = is_not_word(s, i);

		if(current_token)
		{
			printf("start = %d, end = %d, token = %d\n", start, end, current_token);
			make_token(s, start, end);
			start = i;
			if(current_token == REDIR_INSOURCE || current_token == REDIR_APPEND)
			{
				i++;
				end = i;
				continue;
			}
		}
		end = i;
		i++;
	}
	make_token(s, start, end);
}

int main(int argc, char **argv)
{
	t_tree *root;
	char *s = argv[1];
	//looping_string(s);
	loop(s, &root);
	int i = 0;
	enum token_types current_token = is_not_word("pwd", 0);

	// if(current_token == SEP)
	// 	printf("separator");
	// else if(current_token == REDIR_INSOURCE)
	// 	printf("heredoc");
	// else if(current_token == REDIR_APPEND)
	// 	printf("append");
	// else if(current_token == PIPE)
	// 	printf("pipe");
	// else if(current_token == REDIR_OUT)
	// 	printf("output");
	// else if(current_token == REDIR_IN)
	// 	printf("input");
	// else if(current_token == END)
	// 	printf("end of line");
	// else
	// 	printf("probabily the start of a word");

	/*

	*/

}