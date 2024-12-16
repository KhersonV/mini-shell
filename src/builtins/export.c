/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/10 14:25:29 by vmamoten          #+#    #+#             */
/*   Updated: 2024/12/16 12:14:40 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

void	ft_export(char **args, t_info *info)
{
	char	**sorted_env;
	int		i;
	int		i;
	char	*key;
	char	*value;

	if (!args[1])
	{
		sorted_env = copy_envp(info->envp);
		if (!sorted_env)
		{
			perror("minishell: export: failed to copy environment");
			info->exit_status = 1;
			return ;
		}
		sort_env(sorted_env);
		i = 0;
		while (sorted_env[i])
		{
			printf("declare -x %s\n", sorted_env[i]);
			i++;
		}
		free_env(&(t_info){.envp = sorted_env});
		info->exit_status = 0;
		return ;
	}
	i = 1;
	while (args[i])
	{
		key = ft_strndup(args[i], ft_strchr(args[i], '=') - args[i]);
		value = ft_strchr(args[i], '=') ? ft_strdup(ft_strchr(args[i], '=')
				+ 1) : NULL;
		if (is_valid_env_key(key))
		{
			set_env(info, key, value ? value : "");
			info->exit_status = 0;
		}
		else
		{
			printf("minishell: export: `%s': not a valid identifier\n",
				args[i]);
			info->exit_status = 1;
		}
		free(key);
		free(value);
		i++;
	}
}

void	sort_env(char **env)
{
	int		i;
	int		j;
	char	*tmp;

	i = 0;
	while (env[i])
	{
		j = i + 1;
		while (env[j])
		{
			if (ft_strcmp(env[i], env[j]) > 0)
			{
				tmp = env[i];
				env[i] = env[j];
				env[j] = tmp;
			}
			j++;
		}
		i++;
	}
}
