/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/10 14:25:04 by vmamoten          #+#    #+#             */
/*   Updated: 2025/01/13 15:45:52 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

int	validate_cd_input(char **args, char *cwd, t_info *info)
{
	if (!args)
	{
		ft_putendl_fd("minishell: cd: Invalid arguments", 2);
		info->exit_status = 1;
		return (0);
	}
	if (!get_current_directory(cwd))
	{
		info->exit_status = 1;
		return (0);
	}
	return (1);
}

void	ft_cd(char **args, t_info *info)
{
	char	*dir;
	char	cwd[PATH_MAX];

	if (!validate_cd_input(args, cwd, info))
		return ;
	dir = resolve_target_directory(args, info);
	if (!dir)
	{
		info->exit_status = 1;
		return ;
	}
	if (!handle_directory_change(dir, args, info))
		return ;
	if (!update_env_vars(info, cwd))
		info->exit_status = 1;
	else
		info->exit_status = 0;
}
