/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/10 14:25:04 by vmamoten          #+#    #+#             */
/*   Updated: 2025/01/05 13:15:11 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

// Получение текущего каталога
int	get_current_directory(char *cwd)
{
	if (!getcwd(cwd, PATH_MAX))
	{
		perror("minishell: getcwd");
		return (0);
	}
	return (1);
}

// Получение HOME директории
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

// Получение директории для перехода
char	*resolve_target_directory(char **args, t_info *info)
{
	char	*home;
	char	*dir;

	if (!args[1] || ft_strcmp(args[1], "~") == 0)
		return (get_home_directory(info));
	else if (args[1][0] == '~')
	{
		home = get_home_directory(info);
		if (!home)
			return (NULL);
		dir = ft_strjoin(home, args[1] + 1);
		if (!dir)
			ft_putendl_fd("minishell: cd: memory allocation failed", 2);
		return (dir);
	}
	else if (ft_strcmp(args[1], "-") == 0)
	{
		dir = get_env_value(info, "OLDPWD");
		if (!dir || dir[0] == '\0')
			ft_putendl_fd("minishell: cd: OLDPWD not set", STDERR_FILENO);
		else
			ft_putendl_fd(dir, STDOUT_FILENO);
		return (dir);
	}
	return (args[1]);
}

// Обновление переменных окружения
int	update_env_vars(t_info *info, char *old_cwd)
{
	char	cwd[PATH_MAX];

	if (set_env(info, "OLDPWD", old_cwd) == -1)
	{
		ft_putendl_fd("minishell: cd: failed to set OLDPWD", 2);
		return (0);
	}
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

// Переход в директорию
int	try_change_directory(char *dir)
{
	if (chdir(dir) != 0)
	{
		perror("minishell: cd");
		return (0);
	}
	return (1);
}

// Основная функция cd
void	ft_cd(char **args, t_info *info)
{
	char	*dir;
	char	cwd[PATH_MAX];

	if (!args)
	{
		ft_putendl_fd("minishell: cd: Invalid arguments", 2);
		info->exit_status = 1;
		return ;
	}
	if (!get_current_directory(cwd))
	{
		info->exit_status = 1;
		return ;
	}
	dir = resolve_target_directory(args, info);
	if (!dir)
	{
		info->exit_status = 1;
		return ;
	}
	if (!try_change_directory(dir))
	{
		info->exit_status = 1;
		if (args[1] && args[1][0] == '~')
			free(dir);
		return ;
	}
	if (!update_env_vars(info, cwd))
		info->exit_status = 1;
	if (args[1] && args[1][0] == '~')
		free(dir);
	info->exit_status = 0;
}
