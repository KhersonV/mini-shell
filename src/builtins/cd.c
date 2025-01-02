/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/10 14:25:04 by vmamoten          #+#    #+#             */
/*   Updated: 2025/01/02 16:45:36 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

void	ft_cd(char **args, t_info *info)
{
	char	*dir;
	char	cwd[PATH_MAX];
	char	*home;

	dir = NULL;
	// printf("args in cd = %s\n", args[1]);
	// Получение текущего каталога перед сменой
	if (!getcwd(cwd, sizeof(cwd)))
	{
		perror("minishell: getcwd");
		info->exit_status = 1;
		return ;
	}

	// Обработка команды без аргументов, "~", или пустой строки
	if (!args[1] || ft_strcmp(args[1], "~") == 0)
	{
		home = get_env_value(info, "HOME");
		if (!home)
		{
			ft_putendl_fd("minishell: cd: HOME not set", 2);
			info->exit_status = 1;
			return ;
		}
		dir = home;
	}
	else if (ft_strcmp(args[1], "") == 0) // Если аргумент - пустая строка
	{
		// Не меняем каталог, просто выходим из функции
		info->exit_status = 0;
		return ;
	}
	else if (ft_strcmp(args[1], "-") == 0) // Обработка команды cd -
	{
		dir = get_env_value(info, "OLDPWD");
		if (!dir)
		{
			ft_putendl_fd("minishell: cd: OLDPWD not set", 2);
			info->exit_status = 1;
			return ;
		}
		ft_putendl_fd(dir, STDOUT_FILENO);
	}
	else // Обработка команды с указанным аргументом
	{
		dir = args[1];
	}

	// Попытка смены каталога
	if (dir && chdir(dir) != 0)
	{
		perror("minishell: cd");
		info->exit_status = 1;
		return ;
	}

	// Обновление OLDPWD
	if (set_env(info, "OLDPWD", cwd) == -1)
	{
		ft_putendl_fd("minishell: cd: failed to set OLDPWD", 2);
		info->exit_status = 1;
		return ;
	}

	// Получение нового текущего каталога
	if (!getcwd(cwd, sizeof(cwd)))
	{
		perror("minishell: getcwd");
		info->exit_status = 1;
		return ;
	}

	// Обновление PWD
	if (set_env(info, "PWD", cwd) == -1)
	{
		ft_putendl_fd("minishell: cd: failed to set PWD", 2);
		info->exit_status = 1;
		return ;
	}

	info->exit_status = 0;
}
