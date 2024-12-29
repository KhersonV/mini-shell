/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/10 14:25:35 by vmamoten          #+#    #+#             */
/*   Updated: 2024/12/29 14:08:43 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

void	unset_env(char **args, t_info *info)
{
	int i;
	int j;

	if (!info || !args)
		return;

	i = 1; // Начинаем с args[1], так как args[0] — это имя команды "unset"
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
	info->exit_status = 0; // Если всё выполнено успешно
}
