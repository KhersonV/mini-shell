
#include "../../include/minishell.h"

char	*ft_strndup(const char *s, size_t n)
{
	char	*new_str;
	size_t	i;

	if (!s)
		return (NULL);
	new_str = (char *)malloc(n + 1); // Выделяем память с учетом '\0'
	if (!new_str)
		return (NULL);
	i = 0;
	while (i < n && s[i] != '\0') // Копируем до n символов или до конца строки
	{
		new_str[i] = s[i];
		i++;
	}
	new_str[i] = '\0'; // Добавляем завершающий нулевой символ
	return (new_str);
}


void	ft_putstr_fd(char *s, int fd)
{
	int	i;

	i = 0;
	while (s[i])
	{
		ft_putchar_fd(s[i], fd);
		i++;
	}
}

char	*ft_strdup(const char *s1)
{
	int		i;
	int		length;
	char	*copied_string;

	i = 0;
	length = 0;
	while (s1[length])
		length++;
	copied_string = malloc(length + 1);
	if (!copied_string)
		return (NULL);
	while (s1[i])
	{
		copied_string[i] = s1[i];
		i++;
	}
	copied_string[i] = '\0';
	return (copied_string);
}


void	ft_putchar_fd(char c, int fd)
{
	write(fd, &c, 1);
}

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
// 				curr = add_token(curr, buf, "WORD");
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
