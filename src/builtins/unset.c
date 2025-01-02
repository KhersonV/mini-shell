/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/10 14:25:35 by vmamoten          #+#    #+#             */
/*   Updated: 2025/01/02 12:25:15 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

void	unset_env(char **args, t_info *info)
{
	int i;
	int j;

	if (!info || !args)
		return;

	info->exit_status = 0;
	i = 1;
	while (args[i])
	{
		if (!is_valid_env_key(args[i]))
		{
			ft_putstr_fd("minishell: unset: `", 2);
			ft_putstr_fd(args[i], 2);
			ft_putstr_fd("': not a valid identifier\n", 2);
			info->exit_status = 1;
			i++;
			continue;
		}

		j = 0;
		while (info->envp[j])
		{
			if (env_key_compare(info->envp[j], args[i]))
			{
				info->envp = remove_env_entry(info->envp, j);
				break;
			}
			j++;
		}
		i++;
	}
}



int ft_unset(char **args, t_info *info)
{
    int i;
    int index;

    if (!args[1])
        return (0);
    i = 1;
    while (args[i])
    {
        index = -1;
        for (int j = 0; info->envp[j]; j++)
        {
            if (env_key_compare(info->envp[j], args[i]))
            {
                index = j;
                break;
            }
        }
        if (index != -1)
            info->envp = remove_env_entry(info->envp, index);
        i++;
    }
    info->exit_status = 0;
    return (0);
}
