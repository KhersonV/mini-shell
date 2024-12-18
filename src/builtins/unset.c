/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/10 14:25:35 by vmamoten          #+#    #+#             */
/*   Updated: 2024/12/18 12:43:32 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

void	unset_env(t_info *info, char *key)
{
	int	i;

	if (!info || !key || !is_valid_env_key(key))
	{
		ft_putstr_fd("minishell: unset: `", 2);
		ft_putstr_fd(key, 2);
		ft_putstr_fd("': not a valid identifier\n", 2);
		return ;
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
