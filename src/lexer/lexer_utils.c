
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
