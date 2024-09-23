/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lynchsama <lynchsama@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/22 21:23:27 by lynchsama         #+#    #+#             */
/*   Updated: 2024/09/23 20:49:13 by lynchsama        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>


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
	while(start < end)
	{
		printf("%c", s[start]);
		start++;
	}
}

void loop(char *s)
{
	int i = 0;
	int start;
	int end;

	start = i;
	while(s[i] != '\0')
	{
		if(is_not_word(s, i))
		{
			make_token()
			start = i;
		}

		end = i;
		i++;
	}
}

int main(int argc, char **argv)
{
	char *s = argv[1];
	looping_string(s);
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