#include "../minishell.h"

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

size_t	ft_strlen(const char *s)
{
	size_t	i;

	i = 0;
	while (s[i] != '\0')
		i++;
	return (i);
}

int	ft_strcmp(const char *s1, const char *s2)
{
	while (*s1 && (*s1 == *s2))
	{
		s1++;
		s2++;
	}
	return ((unsigned char)*s1 - (unsigned char)*s2);
}


int	ft_strncmp(const char *s1, const char *s2, size_t n)
{
	size_t			i;

	i = 0;
	if (n == 0)
		return (0);
	while (s1[i] != '\0' && (s1[i] == s2[i]) && i < n - 1)
		i++;
	return ((unsigned char)s1[i] - (unsigned char)s2[i]);
}


char	*get_env_value(char **envp, char *var)
{
	int	i;
	int	len;

	len = ft_strlen(var);
	i = 0;
	while (envp[i])
	{
		if (ft_strncmp(envp[i], var, len) == 0 && envp[i][len] == '=')
			return (envp[i] + len + 1);
		i++;
	}
	return (NULL);
}

char	**copy_envp(char **envp)
{
	int		i;
	int		j;
	char	**env_copy;

	i = 0;
	while (envp[i])
		i++;
	env_copy = malloc(sizeof(char *) * (i + 1));
	if (!env_copy)
		return (NULL);
	i = 0;
	while (envp[i])
	{
		env_copy[i] = ft_strdup(envp[i]);
		if (!env_copy[i])
		{
			j = 0;
			while (j < i)
			{
				free(env_copy[j]);
				j++;
			}
			free(env_copy);
			return (NULL);
		}
		i++;
	}
	env_copy[i] = NULL;
	return (env_copy);
}