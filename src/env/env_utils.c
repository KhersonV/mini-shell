/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/11 12:52:06 by vmamoten          #+#    #+#             */
/*   Updated: 2024/12/29 14:34:35 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

int is_valid_env_key(const char *key)
{
    int i = 0;

    if (!key || !key[0])
        return (0);
    while (key[i])
    {
        if (!(ft_isalnum(key[i]) || key[i] == '_'))
            return (0);
        i++;
    }
    return (1);
}

int	env_key_compare(const char *env_entry, const char *key)
{
	int	key_len;

	key_len = ft_strlen(key);
	return (ft_strncmp(env_entry, key, key_len) == 0 && env_entry[key_len] == '=');
}

char	*get_value_from_env(const char *env_entry)
{
	char	*equals;

	equals = ft_strchr(env_entry, '=');
	if (!equals)
		return (NULL);
	return (ft_strdup(equals + 1));
}


char *get_env_value_direct(char **envp, const char *key)
{
    int i = 0;
    size_t key_len = ft_strlen(key);

    while (envp[i])
    {
        if (ft_strncmp(envp[i], key, key_len) == 0 && envp[i][key_len] == '=')
            return (ft_strdup(envp[i] + key_len + 1));
        i++;
    }
    return (NULL);
}

char	*create_env_entry(const char *key, const char *value)
{
	char	*entry;
	size_t	len_key;
	size_t	len_value;
	size_t	len;

	if (!key || !value)
		return (NULL);
	len_key = ft_strlen(key);
	len_value = ft_strlen(value);
	len = len_key + len_value + 2;
	entry = (char *)malloc(len);
	if (!entry)
		exit(EXIT_FAILURE);
	ft_strlcpy(entry, key, len_key + 1);
	entry[len_key] = '=';
	ft_strlcpy(entry + len_key + 1, value, len - len_key - 1);
	return (entry);
}

char	**append_env_entry(char **env, const char *entry)
{
	int		i;
	int		count;
	char	**new_env;

	i = 0;
	count = 0;
	while (env[count])
		count++;
	new_env = (char **)malloc((count + 2) * sizeof(char *));
	if (!new_env)
		exit(EXIT_FAILURE);
	while (i < count)
	{
		new_env[i] = env[i];
		i++;
	}
	new_env[i] = ft_strdup(entry);
	new_env[i + 1] = NULL;
	free(env);
	return (new_env);
}

char	**remove_env_entry(char **env, int index)
{
	int		count;
	char	**new_env;
	int		i;
	int		j;

	i = 0;
	j = 0;
	count = 0;
	while (env[count])
		count++;
	new_env = (char **)malloc(count * sizeof(char *));
	if (!new_env)
		exit(EXIT_FAILURE);
	while (env[i])
	{
		if (i != index)
			new_env[j++] = env[i];
		else
			free(env[i]);
		i++;
	}
	new_env[j] = NULL;
	free(env);
	return (new_env);
}

void	free_env_array(char **env)
{
    int i;

    if (!env)
        return;
    i = 0;
    while (env[i])
    {
        free(env[i]);
        i++;
    }
    free(env);
}
