/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_builtin.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/10 14:25:23 by vmamoten          #+#    #+#             */
/*   Updated: 2024/12/16 14:14:21 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

void	ft_env(char **args, t_info *info)
{
	char	**env;
	int		i;

	if (args[1])
	{
		write(2, "env: too many arguments\n", 24);
		info->exit_status = 1;
		return ;
	}
	env = env_to_array(info);
	if (!env)
	{
		perror("minishell: env: failed to copy environment");
		info->exit_status = 1;
		return ;
	}
	i = 0;
	while (env[i])
	{
		printf("%s\n", env[i]);
		i++;
	}
	free_env_array(env);
	info->exit_status = 0;
}
