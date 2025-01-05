/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/10 14:25:04 by vmamoten          #+#    #+#             */
/*   Updated: 2025/01/05 16:26:55 by vmamoten         ###   ########.fr       */
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
