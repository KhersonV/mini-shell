/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd_utils.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/06 17:47:58 by vmamoten          #+#    #+#             */
/*   Updated: 2025/01/06 17:53:51 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

int	get_current_directory(char *cwd)
{
	if (!getcwd(cwd, PATH_MAX))
	{
		perror("minishell: getcwd");
		return (0);
	}
	return (1);
}

char	*get_home_directory(t_info *info)
{
	char	*home;

	home = get_env_value(info, "HOME");
	if (!home)
	{
		ft_putendl_fd("minishell: cd: HOME not set", 2);
		return (NULL);
	}
	return (home);
}

char	*get_directory_from_tilde(char *arg, t_info *info)
{
	char	*home;
	char	*dir;

	home = get_home_directory(info);
	if (!home)
		return (NULL);
	dir = ft_strjoin(home, arg + 1);
	if (!dir)
		ft_putendl_fd("minishell: cd: memory allocation failed", 2);
	return (dir);
}

char	*get_directory_from_dash(char *arg, t_info *info)
{
	char	*dir;

	if (ft_strcmp(arg, "-") == 0)
	{
		dir = get_env_value(info, "OLDPWD");
		if (!dir || dir[0] == '\0')
		{
			ft_putendl_fd("minishell: cd: OLDPWD not set", STDERR_FILENO);
			return (NULL);
		}
		ft_putendl_fd(dir, STDOUT_FILENO);
		return (dir);
	}
	return (get_home_directory(info));
}

char	*resolve_target_directory(char **args, t_info *info)
{
	if (!args[1] || ft_strcmp(args[1], "~") == 0)
		return (get_home_directory(info));
	if (args[1][0] == '~')
		return (get_directory_from_tilde(args[1], info));
	if (ft_strcmp(args[1], "-") == 0 || ft_strcmp(args[1], "--") == 0)
		return (get_directory_from_dash(args[1], info));
	return (args[1]);
}
