/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/10 14:25:35 by vmamoten          #+#    #+#             */
/*   Updated: 2024/12/16 14:32:19 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */



#include "../../include/minishell.h"

void	unset_env(t_info *info, const char *key)
{
	int	i;

	 if (!info || !key || !is_valid_env_key(key))
    {
        printf(stderr, "minishell: unset: `%s': not a valid identifier\n", key);
        return;
    }
	i = 0;
	while (info->envp[i])
	{
		if (env_key_compare(info->envp[i], key))
		{
			free(info->envp[i]);
			info->envp = remove_env_entry(info->envp, i);
			break ;
		}
		i++;
	}
}
