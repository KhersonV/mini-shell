/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/11 12:43:01 by vmamoten          #+#    #+#             */
/*   Updated: 2024/12/19 13:23:20 by vmamoten         ###   ########.fr       */
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
