/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/11 12:43:01 by vmamoten          #+#    #+#             */
/*   Updated: 2025/01/08 18:18:22 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

char	**allocate_env_copy(char **envp, int *size)
{
	int		i;
	char	**env_copy;

	if (!envp)
		return (NULL);
	i = 0;
	while (envp[i])
		i++;
	*size = i;
	env_copy = malloc(sizeof(char *) * (i + 1));
	if (!env_copy)
		return (NULL);
	return (env_copy);
}

void	free_partial_env_copy(char **env_copy, int count)
{
	int	j;

	j = 0;
	while (j < count)
	{
		free(env_copy[j]);
		j++;
	}
	free(env_copy);
}

char	*validate_env_key(t_info *info, const char *key, const char *value)
{
	char	*new_entry;

	if (!info || !key || !value || !is_valid_env_key(key))
	{
		write(STDERR_FILENO, "minishell: export: `", 21);
		write(STDERR_FILENO, key, ft_strlen(key));
		write(STDERR_FILENO, "': not a valid identifier\n", 26);
		if (info)
			info->exit_status = 1;
		return (NULL);
	}
	new_entry = create_env_entry(key, value);
	if (!new_entry)
	{
		perror("minishell: export: failed to create env entry");
		if (info)
			info->exit_status = 1;
		return (NULL);
	}
	return (new_entry);
}

int	update_env_entry(t_info *info, const char *key, char *new_entry)
{
	int	i;

	i = 0;
	while (info->envp[i])
	{
		if (env_key_compare(info->envp[i], key))
		{
			free(info->envp[i]);
			info->envp[i] = new_entry;
			info->exit_status = 0;
			return (0);
		}
		i++;
	}
	info->envp = append_env_entry(info->envp, new_entry);
	if (!info->envp)
	{
		free(new_entry);
		perror("minishell: export: failed to append env entry");
		info->exit_status = 1;
		return (-1);
	}
	info->exit_status = 0;
	return (0);
}

int	set_env(t_info *info, const char *key, const char *value)
{
	char	*new_entry;

	new_entry = validate_env_key(info, key, value);
	if (!new_entry)
		return (-1);
	return (update_env_entry(info, key, new_entry));
}
