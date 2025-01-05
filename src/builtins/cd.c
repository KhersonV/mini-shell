/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/10 14:25:04 by vmamoten          #+#    #+#             */
/*   Updated: 2025/01/05 13:05:05 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

void	ft_cd(char **args, t_info *info)
{
	char	*dir = NULL;
	char	cwd[PATH_MAX];
	char	*home;

	// Проверка на NULL для args
	if (!args)
	{
		ft_putendl_fd("minishell: cd: Invalid arguments", 2);
		info->exit_status = 1;
		return;
	}

	// Получение текущего каталога
	if (!getcwd(cwd, sizeof(cwd)))
	{
		perror("minishell: getcwd");
		info->exit_status = 1;
		return;
	}

	// Обработка случая без аргументов или с "~"
	if (!args[1] || ft_strcmp(args[1], "~") == 0)
	{
		home = get_env_value(info, "HOME");
		if (!home)
		{
			ft_putendl_fd("minishell: cd: HOME not set", 2);
			info->exit_status = 1;
			return;
		}
		dir = home;
	}
	// Обработка случая, когда путь начинается с '~'
	else if (args[1][0] == '~')
	{
		home = get_env_value(info, "HOME");
		if (!home)
		{
			ft_putendl_fd("minishell: cd: HOME not set", 2);
			info->exit_status = 1;
			return;
		}
		dir = ft_strjoin(home, args[1] + 1); // Объединяем HOME с остатком пути
		if (!dir)
		{
			ft_putendl_fd("minishell: cd: memory allocation failed", 2);
			info->exit_status = 1;
			return;
		}
	}
	// Обработка "cd -"
	else if (ft_strcmp(args[1], "-") == 0)
	{
		dir = get_env_value(info, "OLDPWD");
		if (!dir || dir[0] == '\0')
		{
			ft_putendl_fd("minishell: cd: OLDPWD not set", STDERR_FILENO);
			info->exit_status = 1;
			return;
		}
		ft_putendl_fd(dir, STDOUT_FILENO);
	}
	else // Указан путь
	{
		dir = args[1];
	}

	// Попытка смены каталога
	if (dir && chdir(dir) != 0)
	{
		perror("minishell: cd");
		info->exit_status = 1;
		if (args[1] && args[1][0] == '~') // Освобождаем память, если использовали ft_strjoin
			free(dir);
		return;
	}

	// Обновление OLDPWD
	if (set_env(info, "OLDPWD", cwd) == -1)
	{
		ft_putendl_fd("minishell: cd: failed to set OLDPWD", 2);
		info->exit_status = 1;
		if (args[1] && args[1][0] == '~')
			free(dir);
		return;
	}

	// Получение нового текущего каталога
	if (!getcwd(cwd, sizeof(cwd)))
	{
		perror("minishell: getcwd");
		info->exit_status = 1;
		if (args[1] && args[1][0] == '~')
			free(dir);
		return;
	}

	// Обновление PWD
	if (set_env(info, "PWD", cwd) == -1)
	{
		ft_putendl_fd("minishell: cd: failed to set PWD", 2);
		info->exit_status = 1;
		if (args[1] && args[1][0] == '~')
			free(dir);
		return;
	}

	// Освобождаем память, если использовали ft_strjoin
	if (args[1] && args[1][0] == '~')
		free(dir);

	info->exit_status = 0;
}
