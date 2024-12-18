/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/11 12:43:01 by vmamoten          #+#    #+#             */
/*   Updated: 2024/12/18 12:05:30 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "../../include/minishell.h"

char	**copy_envp(char **envp)
{
	int		i;
	int		j;
	char	**env_copy;

	if (!envp)
		return (NULL);
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

void	init_env(t_info *info, char **envp)
{
	info->envp = copy_envp(envp);
	if (!info->envp)
		exit(EXIT_FAILURE);
}

char	*get_env_value(t_info *info, const char *key)
{
	int	i;

	i = 0;
	while (info->envp[i])
	{
		if (env_key_compare(info->envp[i], key))
			return (get_value_from_env(info->envp[i]));
		i++;
	}
	return (NULL);
}

int set_env(t_info *info, const char *key, const char *value)
{
    int i;
    char *new_entry;

    if (!info || !key || !value || !is_valid_env_key(key))
    {
        fprintf(stderr, "minishell: export: `%s': not a valid identifier\n", key);
        info->exit_status = 1;
        return -1; // Указываем, что произошла ошибка
    }

    new_entry = create_env_entry(key, value);
    if (!new_entry)
    {
        perror("minishell: export: failed to create env entry");
        info->exit_status = 1;
        return -1; // Указываем, что произошла ошибка
    }

    i = 0;
    while (info->envp[i])
    {
        if (env_key_compare(info->envp[i], key))
        {
            free(info->envp[i]);
            info->envp[i] = new_entry;
            info->exit_status = 0;
            return 0; // Указываем, что выполнение успешно
        }
        i++;
    }
    info->envp = append_env_entry(info->envp, new_entry);
    info->exit_status = 0;
    return 0; // Указываем, что выполнение успешно
}

char	**env_to_array(t_info *info)
{
	return (copy_envp(info->envp));
}

void	free_env(t_info *info)
{
	int	i;

	if (!info || !info->envp)
		return;
	i = 0;
	while (info->envp[i])
	{
		free(info->envp[i]);
		i++;
	}
	free(info->envp);
	info->envp = NULL;
}

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