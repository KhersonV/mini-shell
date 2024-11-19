/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lynchsama <lynchsama@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/22 21:23:27 by lynchsama         #+#    #+#             */
/*   Updated: 2024/11/19 20:50:10 by lynchsama        ###   ########.fr       */
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
	else if (current_char == ' ' || (current_char >= 9 && current_char <= 13))
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
		if (s[i] == '|' || s[i] == '<' || s[i] == '>' || s[i] == ' ' || (s[i] >= 9 && s[i] <= 13))
		{
			if (buf_index > 0)
			{
				buf[buf_index] = '\0';
				curr = add_token(curr, buf, "WORD");
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
				curr = add_token(curr, buf, "WORD");
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
				curr = add_token(curr, buf, "WORD");
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
				curr = add_token(curr, variable_buffer, "VAR");
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
		curr = add_token(curr, buf, "WORD");
		buf_index = 0;
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

/*(
1. Empty Input
c
Copy code
char input[] = "";
Expected Tokens:

No tokens should be produced since the input is empty.

2. Input with Only Operators
c
Copy code
char input[] = "| < << > >>";
Expected Tokens:

yaml
Copy code
Token: |, Type: PIPE
Token: [], Type: TOKEN_SPACE
Token: <, Type: REDIR_IN
Token: [], Type: TOKEN_SPACE
Token: <<, Type: REDIR_INSOURCE
Token: [], Type: TOKEN_SPACE
Token: >, Type: REDIR_OUT
Token: [], Type: TOKEN_SPACE
Token: >>, Type: REDIR_APPEND
3. Input with Only Variables
c
Copy code
char input[] = "$var1 $var2 $var_3";
Expected Tokens:

yaml
Copy code
Token: var1, Type: VAR
Token: [], Type: TOKEN_SPACE
Token: var2, Type: VAR
Token: [], Type: TOKEN_SPACE
Token: var_3, Type: VAR
4. Variables Adjacent to Words and Operators
c
Copy code
char input[] = "echo$USER|grep$HOME";
Expected Tokens:

yaml
Copy code
Token: echo, Type: WORD
Token: USER, Type: VAR
Token: |, Type: PIPE
Token: grep, Type: WORD
Token: HOME, Type: VAR
5. Variables with Invalid Names
c
Copy code
char input[] = "$9invalid $%notvar";
Expected Tokens:

yaml
Copy code
Token: 9invalid, Type: WORD
Token: [], Type: TOKEN_SPACE
Token: %notvar, Type: WORD
Note: Since variable names cannot start with a digit or %, these are treated as WORD tokens.

6. Quoted Strings with Embedded Quotes
c
Copy code
char input[] = "'It''s a test' \"She said, \\\"Hello\\\"\"";
Expected Tokens:

yaml
Copy code
Token: It's a test, Type: FIELD
Token: [], Type: TOKEN_SPACE
Token: She said, \"Hello\", Type: EXP_FIELD
Note: Your lexer may not handle escape sequences. Adjust the input accordingly if escape sequences are not supported.

7. Unclosed Quotes
c
Copy code
char input[] = "echo 'This is unclosed string";
Expected Behavior:

The lexer should detect the unclosed quote and report a syntax error.

8. Back-to-Back Operators
c
Copy code
char input[] = "cmd1||cmd2";
Expected Tokens:

yaml
Copy code
Token: cmd1, Type: WORD
Token: |, Type: PIPE
Token: |, Type: PIPE
Token: cmd2, Type: WORD
9. Operators Without Spaces
c
Copy code
char input[] = "cat>file.txt";
Expected Tokens:

yaml
Copy code
Token: cat, Type: WORD
Token: >, Type: REDIR_OUT
Token: file.txt, Type: WORD
10. Words with Special Characters
c
Copy code
char input[] = "filename_with-special.characters";
Expected Tokens:

yaml
Copy code
Token: filename_with-special.characters, Type: WORD
11. Multiple Spaces and Tabs
c
Copy code
char input[] = "echo    'hello' \t\t   world";
Expected Tokens:

yaml
Copy code
Token: echo, Type: WORD
Token: [], Type: TOKEN_SPACE
Token: hello, Type: FIELD
Token: [], Type: TOKEN_SPACE
Token: world, Type: WORD
12. Input with Newlines
c
Copy code
char input[] = "echo 'line1\nline2'";
Expected Tokens:

yaml
Copy code
Token: echo, Type: WORD
Token: [], Type: TOKEN_SPACE
Token: line1
line2, Type: FIELD
13. Variables Inside Quotes
c
Copy code
char input[] = "'$VAR inside single quotes' \"$VAR inside double quotes\"";
``**

**Expected Tokens:**

Token: $VAR inside single quotes, Type: FIELD Token: [], Type: TOKEN_SPACE Token: $VAR inside double quotes, Type: EXP_FIELD

yaml
Copy code

---

### **14. Multiple Variables in a Row**

```c
char input[] = "$VAR1$VAR2$VAR3";
Expected Tokens:

yaml
Copy code
Token: VAR1, Type: VAR
Token: VAR2, Type: VAR
Token: VAR3, Type: VAR
15. Variables Adjacent to Numbers and Special Characters
c
Copy code
char input[] = "$VAR123abc$VAR_456$VAR$";
Expected Tokens:

yaml
Copy code
Token: VAR123abc, Type: VAR
Token: VAR_456, Type: VAR
Token: VAR, Type: VAR
Token: $, Type: WORD
Note: The last $ without a valid variable name is treated as a WORD.

16. Unmatched Operators
c
Copy code
char input[] = "echo >| file";
Expected Tokens:

yaml
Copy code
Token: echo, Type: WORD
Token: [], Type: TOKEN_SPACE
Token: >, Type: REDIR_OUT
Token: |, Type: PIPE
Token: [], Type: TOKEN_SPACE
Token: file, Type: WORD
17. Nested Quotes
c
Copy code
char input[] = "\"This is 'nested' quotes\"";
Expected Tokens:

python
Copy code
Token: This is 'nested' quotes, Type: EXP_FIELD
18. Input Ending with an Operator
c
Copy code
char input[] = "command >";
Expected Tokens:

yaml
Copy code
Token: command, Type: WORD
Token: [], Type: TOKEN_SPACE
Token: >, Type: REDIR_OUT
19. Input Starting with an Operator
c
Copy code
char input[] = "> output.txt";
Expected Tokens:

yaml
Copy code
Token: >, Type: REDIR_OUT
Token: [], Type: TOKEN_SPACE
Token: output.txt, Type: WORD
20. Mixed Quoted and Unquoted Text
c
Copy code
char input[] = "echo \"Hello\"World'!' $USER";
Expected Tokens:

yaml
Copy code
Token: echo, Type: WORD
Token: [], Type: TOKEN_SPACE
Token: Hello, Type: EXP_FIELD
Token: World, Type: WORD
Token: !, Type: FIELD
Token: [], Type: TOKEN_SPACE
Token: USER, Type: VAR
21. Input with Special Characters in Words
c
Copy code
char input[] = "echo file@name#with$pecial%chars";
Expected Tokens:

yaml
Copy code
Token: echo, Type: WORD
Token: [], Type: TOKEN_SPACE
Token: file@name#with, Type: WORD
Token: pecial%chars, Type: WORD
Note: $pecial is treated as $ followed by pecial, but since p is a valid start for a variable, it's treated as VAR.

22. Command Substitution
c
Copy code
char input[] = "echo $(date)";
Expected Tokens:

yaml
Copy code
Token: echo, Type: WORD
Token: [], Type: TOKEN_SPACE
Token: $(date), Type: WORD
Note: Since command substitution is not handled, it's treated as a WORD.

23. Input with No Spaces Between Words and Variables
c
Copy code
char input[] = "echoHello$USER";
Expected Tokens:

yaml
Copy code
Token: echoHello, Type: WORD
Token: USER, Type: VAR
24. Input with Multiple Spaces
c
Copy code
char input[] = "    echo    'test'   ";
Expected Tokens:

yaml
Copy code
Token: [], Type: TOKEN_SPACE
Token: echo, Type: WORD
Token: [], Type: TOKEN_SPACE
Token: test, Type: FIELD
Token: [], Type: TOKEN_SPACE
25. Input with Only Spaces
c
Copy code
char input[] = "    ";
Expected Tokens:

yaml
Copy code
Token: [], Type: TOKEN_SPACE
26. Inputs with Operators and Variables Mixed
c
Copy code
char input[] = "$VAR|$ANOTHER_VAR>$OUTPUT";
Expected Tokens:

yaml
Copy code
Token: VAR, Type: VAR
Token: |, Type: PIPE
Token: ANOTHER_VAR, Type: VAR
Token: >, Type: REDIR_OUT
Token: OUTPUT, Type: VAR
27. Unclosed Variables
c
Copy code
char input[] = "$";
Expected Tokens:

yaml
Copy code
Token: $, Type: WORD
28. Input with Special Characters After Variables
c
Copy code
char input[] = "$VAR!$VAR?$$";
Expected Tokens:

yaml
Copy code
Token: VAR, Type: VAR
Token: !, Type: WORD
Token: VAR, Type: VAR
Token: ?, Type: WORD
Token: $, Type: WORD
29. Input with Mixed Single and Double Quotes
c
Copy code
char input[] = "\"Double 'Single' Double\" 'Single \"Double\" Single'";
Expected Tokens:

vbnet
Copy code
Token: Double 'Single' Double, Type: EXP_FIELD
Token: [], Type: TOKEN_SPACE
Token: Single "Double" Single, Type: FIELD
30. Input with Backslash Characters
c
Copy code
char input[] = "echo backslash\\test";
Expected Tokens:

yaml
Copy code
Token: echo, Type: WORD
Token: [], Type: TOKEN_SPACE
Token: backslash\test, Type: WORD
Note: Backslashes are included in the WORD token since escape sequences are not processed.

*/