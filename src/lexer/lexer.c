/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/22 21:23:27 by lynchsama         #+#    #+#             */
/*   Updated: 2024/12/18 13:04:14 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"


#include "../../include/minishell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Создает новый токен
static t_token *create_token_node(char *str, int type)
{
    t_token *new_node;

    new_node = (t_token *)malloc(sizeof(t_token));
    if (!new_node)
        return (NULL);
    new_node->str = ft_strdup(str);
    new_node->type = type;
    new_node->next = NULL;
    new_node->prev = NULL;
    return (new_node);
}

// Добавляет токен в связный список
static t_token *add_token(t_token *node, char *str, int type)
{
    t_token *new_node;
    t_token *curr;

    new_node = create_token_node(str, type);
    if (!new_node)
        return (NULL);
    if (!node)
        return (new_node);
    curr = node;
    while (curr->next)
        curr = curr->next;
    curr->next = new_node;
    new_node->prev = curr;
    return (node);
}

// Дублирует строку с ограничением длины
static char *dup_field(char *s, int len)
{
    char *out;
    int i;

    out = malloc((sizeof(char) * len) + 1);
    if (!out)
        return (NULL);
    i = 0;
    while (i < len)
    {
        out[i] = s[i];
        i++;
    }
    out[i] = '\0';
    return (out);
}

// Извлекает содержимое кавычек
static int extract_field(char *s, t_token **tokens)
{
    int len = 0;
    char quote = *s;
    char *field;

    s++;
    while (s[len] && s[len] != quote)
        len++;
    if (s[len] == '\0')
    {
        ft_putstr_fd("Syntax error: unmatched quote\n", 2);
        return (-1);
    }
    field = dup_field(s, len);
    if (!field)
        return (-1);
    *tokens = add_token(*tokens, field, TOKEN_WORD);
    free(field);
    return (len + 2);
}

// Проверяет, является ли символ оператором
static int is_operator(char c)
{
    if (c == '|' || c == '<' || c == '>')
        return (1);
    return (0);
}

// Определяет тип оператора
static int get_operator_type(char c, char next)
{
    if (c == '|' && next != '|')
        return (TOKEN_PIPE);
    if (c == '<' && next == '<')
        return (TOKEN_HEREDOC);
    if (c == '<')
        return (TOKEN_REDIRECT_IN);
    if (c == '>' && next == '>')
        return (TOKEN_REDIRECT_APPEND);
    if (c == '>')
        return (TOKEN_REDIRECT_OUT);
    return (TOKEN_WORD);
}
t_token *tokenize(char *s)
{
    t_token *tokens = NULL;
    char buf[256];
    int buf_index = 0;
    int i = 0;

    while (s[i])
    {
        if (is_operator(s[i]))
        {
            if (buf_index > 0)
            {
                buf[buf_index] = '\0';
                tokens = add_token(tokens, buf, TOKEN_WORD);
                buf_index = 0;
            }
            int type = get_operator_type(s[i], s[i + 1]);
            tokens = add_token(tokens, ft_strndup(&s[i], (type == TOKEN_HEREDOC || type == TOKEN_REDIRECT_APPEND) ? 2 : 1), type);
            i += (type == TOKEN_HEREDOC || type == TOKEN_REDIRECT_APPEND) ? 2 : 1;
        }
        else if (s[i] == '\'' || s[i] == '"')
        {
            int len = extract_field(&s[i], &tokens);
            if (len == -1)
                return (NULL);
            i += len;
        }
        else if (s[i] == ' ' || s[i] == '\t')
        {
            if (buf_index > 0)
            {
                buf[buf_index] = '\0';
                tokens = add_token(tokens, buf, TOKEN_WORD);
                buf_index = 0;
            }
            i++;
        }
        else
        {
            buf[buf_index++] = s[i++];
        }
    }
    if (buf_index > 0)
    {
        buf[buf_index] = '\0';
        tokens = add_token(tokens, buf, TOKEN_WORD);
    }
    return (tokens);
}


// Выводит токены для отладки
void print_tokens(t_token *tokens)
{
    while (tokens)
    {
        printf("Token: %s, Type: %d\n", tokens->str, tokens->type);
        tokens = tokens->next;
    }
}





// // Проверка символов на тип токена
// int	is_not_word(char *str, int i)
// {
// 	if ((str[i] > 8 && str[i] < 14) || (str[i] == 32))
// 		return (TOKEN_SPACE);
// 	else if (str[i] == '<' && str[i + 1] == '<')
// 		return (TOKEN_HEREDOC);
// 	else if (str[i] == '>' && str[i + 1] == '>')
// 		return (TOKEN_REDIRECT_APPEND);
// 	else if (str[i] == '|')
// 		return (TOKEN_PIPE);
// 	else if (str[i] == '>')
// 		return (TOKEN_REDIRECT_OUT);
// 	else if (str[i] == '<')
// 		return (TOKEN_REDIRECT_IN);
// 	else if (str[i] == '\0')
// 		return (TOKEN_END);
// 	else
// 		return (TOKEN_WORD);
// }

// t_tree	*create_token_node(char *name, int type)
// {
// 	t_tree	*new_node;

// 	new_node = (t_tree *)malloc(sizeof(t_tree));
// 	if (!new_node)
// 		return (NULL);
// 	new_node->name = ft_strdup(name);
// 	new_node->type = type;
// 	new_node->next = NULL;
// 	new_node->prev = NULL;
// 	return (new_node);
// }

// t_tree	*add_token(t_tree *node, char *name, int type)
// {
// 	t_tree	*new_node;
// 	t_tree	*curr;

// 	new_node = create_token_node(name, type);
// 	if (!new_node)
// 		return (NULL);
// 	if (!node)
// 		return (new_node);
// 	curr = node;
// 	while (curr->next)
// 		curr = curr->next;
// 	curr->next = new_node;
// 	new_node->prev = curr;
// 	return (node);
// }

// char	*dup_field(char *s, int len)
// {
// 	char	*out;
// 	int		i;

// 	out = malloc((sizeof(char) * len) + 1);
// 	if (!out)
// 		return (NULL);
// 	i = 0;
// 	while (i < len)
// 	{
// 		out[i] = s[i];
// 		i++;
// 	}
// 	out[i] = '\0';
// 	return (out);
// }

// int	extract_field(char *s, t_tree *element)
// {
// 	int		len;
// 	char	quote;
// 	char	*field;

// 	len = 0;
// 	quote = *s;
// 	s++;
// 	while (s[len] != quote && s[len] != '\0')
// 		len++;
// 	if (s[len] == '\0')
// 	{
// 		ft_putstr_fd("Syntax error: unmatched quote\n", 2);
// 		exit(1);
// 	}
// 	field = dup_field(s, len);
// 	if (quote == '"')
// 		add_token(element, field, TOKEN_WORD);
// 	else if (quote == '\'')
// 		add_token(element, field, TOKEN_WORD);
// 	return (len + 2);
// }

// t_tree	*tokenize(char *s)
// {
// 	t_tree	*curr;
// 	char	buf[256];
// 	int		i;
// 	int		buf_index;

// 	curr = NULL;
// 	i = 0;
// 	buf_index = 0;
// 	while (s[i])
// 	{
// 		if (is_not_word(s, i))
// 		{
// 			if (buf_index > 0)
// 			{
// 				buf[buf_index] = '\0';
// 				curr = add_token(curr, buf, TOKEN_WORD);
// 				buf_index = 0;
// 			}
// 			if (s[i] == '|' || s[i] == '<' || s[i] == '>')
// 				curr = add_token(curr, &s[i], is_not_word(s, i));
// 			i++;
// 		}
// 		else if (s[i] == '\'' || s[i] == '"')
// 			i += extract_field(&s[i], curr);
// 		else
// 			buf[buf_index++] = s[i++];
// 	}
// 	if (buf_index > 0)
// 	{
// 		buf[buf_index] = '\0';
// 		curr = add_token(curr, buf, TOKEN_WORD);
// 	}
// 	return (curr);
// }

// void	print_tokens(t_tree *node)
// {
// 	while (node)
// 	{
// 		printf("Token: %s, Type: %d\n", node->name, node->type);
// 		node = node->next;
// 	}
// }

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