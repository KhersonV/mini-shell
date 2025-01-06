/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/11 12:52:06 by vmamoten          #+#    #+#             */
/*   Updated: 2025/01/06 19:15:38 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

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

char	**copy_envp(char **envp)
{
	int		i;
	int		size;
	char	**env_copy;

	size = 0;
	env_copy = allocate_env_copy(envp, &size);
	if (!env_copy)
		return (NULL);
	i = 0;
	while (i < size)
	{
		env_copy[i] = ft_strdup(envp[i]);
		if (!env_copy[i])
		{
			free_partial_env_copy(env_copy, i);
			return (NULL);
		}
		i++;
	}
	env_copy[i] = NULL;
	return (env_copy);
}

void	remove_oldpwd(char ***envp)
{
	int	i;

	i = 0;
	while ((*envp)[i])
	{
		if (env_key_compare((*envp)[i], "OLDPWD"))
		{
			*envp = remove_env_entry(*envp, i);
			break ;
		}
		i++;
	}
}

int	calculate_shlvl(char *shlvl_value)
{
	int		shlvl;
	char	*str;

	str = "minishell: warning: shell level too high, resetting to 1\n";
	if (shlvl_value)
	{
		shlvl = ft_atoi(shlvl_value);
		free(shlvl_value);
		shlvl++;
		if (shlvl > 999)
		{
			ft_putstr_fd(str, STDERR_FILENO);
			return (1);
		}
	}
	else
		shlvl = 1;
	return (shlvl);
}

void	shlvl_to_string(int shlvl, char *buffer)
{
	int		i;
	int		j;
	char	tmp;

	i = 0;
	while (shlvl > 0)
	{
		buffer[i++] = (shlvl % 10) + '0';
		shlvl /= 10;
	}
	buffer[i] = '\0';
	j = 0;
	while (j < i / 2)
	{
		tmp = buffer[j];
		buffer[j] = buffer[i - j - 1];
		buffer[i - j - 1] = tmp;
		j++;
	}
}
