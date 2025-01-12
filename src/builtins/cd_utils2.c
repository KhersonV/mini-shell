/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd_utils2.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/06 17:50:32 by vmamoten          #+#    #+#             */
/*   Updated: 2025/01/12 18:20:31 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

int	update_env_vars(t_info *info, char *old_cwd)
{
	char	cwd[PATH_MAX];
	if (set_env(info, "OLDPWD", old_cwd) == -1)
	{
		ft_putendl_fd("minishell: cd: failed to set OLDPWD", STDERR_FILENO);
		return (0);
	}
	if (!getcwd(cwd, sizeof(cwd)))
	{
		perror("minishell: getcwd");
		return (0);
	}
	if (set_env(info, "PWD", cwd) == -1)
	{
		ft_putendl_fd("minishell: cd: failed to set PWD", STDERR_FILENO);
		return (0);
	}
	return (1);
}

int	handle_directory_change(char *dir, char **args, t_info *info)
{
	if (chdir(dir) != 0)
	{
		perror("minishell: cd");
		info->exit_status = 1;
		if (args[1] && (args[1][0] == '~' || args[1][0] == '\0'))
			free(dir);
		return (0);
	}
	if (args[1] && (args[1][0] == '~' || args[1][0] == '\0'))
		free(dir);
	return (1);
}
