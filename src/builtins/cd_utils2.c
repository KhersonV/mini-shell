/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd_utils2.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/06 17:50:32 by vmamoten          #+#    #+#             */
/*   Updated: 2025/01/06 17:51:00 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

int	update_oldpwd(t_info *info, char *old_cwd)
{
	if (set_env(info, "OLDPWD", old_cwd) == -1)
	{
		ft_putendl_fd("minishell: cd: failed to set OLDPWD", 2);
		return (0);
	}
	return (1);
}

int	update_pwd(t_info *info)
{
	char	cwd[PATH_MAX];

	if (!getcwd(cwd, PATH_MAX))
	{
		perror("minishell: getcwd");
		return (0);
	}
	if (set_env(info, "PWD", cwd) == -1)
	{
		ft_putendl_fd("minishell: cd: failed to set PWD", 2);
		return (0);
	}
	return (1);
}

int	update_env_vars(t_info *info, char *old_cwd)
{
	if (!update_oldpwd(info, old_cwd))
		return (0);
	if (!update_pwd(info))
		return (0);
	return (1);
}

int	try_change_directory(char *dir)
{
	if (chdir(dir) != 0)
	{
		perror("minishell: cd");
		return (0);
	}
	return (1);
}

int	handle_directory_change(char *dir, char **args, t_info *info)
{
	if (!try_change_directory(dir))
	{
		info->exit_status = 1;
		if (args[1] && args[1][0] == '~')
			free(dir);
		return (0);
	}
	if (args[1] && args[1][0] == '~')
		free(dir);
	return (1);
}
