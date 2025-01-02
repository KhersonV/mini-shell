/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/10 14:25:04 by vmamoten          #+#    #+#             */
/*   Updated: 2025/01/02 14:17:47 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

static void	print_error(char *path)
{
    ft_putstr_fd("minishell: cd: ", 2);
    perror(path);
}

static int	update_oldpwd(t_info *info, char *cwd)
{
    if (set_env(info, "OLDPWD", cwd) == -1)
    {
        ft_putendl_fd("minishell: cd: не удалось обновить OLDPWD", 2);
        return (-1);
    }
    return (0);
}

void ft_cd(char **args, t_info *info)
{
    char *dir;
    char cwd[PATH_MAX];
    char *home;


    // Получение текущего каталога перед сменой
    if (!getcwd(cwd, sizeof(cwd)))
    {
        perror("minishell: getcwd");
        info->exit_status = 1;
        return;
    }

    // Обработка команды cd без аргументов, cd ~ или cd ""
    if (!args[1] || ft_strcmp(args[1], "~") == 0 || args[1][0] == '\0')
    {
        home = get_env_value(info, "HOME");
		printf("home = %s\n", home);
		printf("info->home = %s\n", info->home);
        if (!home)
        {
            if (info->home && ft_strcmp(args[1], "~" ))
                home = info->home;
            else
            {
                ft_putendl_fd("minishell: cd: HOME not set", 2);
                info->exit_status = 1;
                return;
            }
        }
        dir = home;
    }
    else if (strcmp(args[1], "-") == 0) // Обработка команды cd -
    {
        dir = get_env_value(info, "OLDPWD");
        if (!dir)
        {
            ft_putendl_fd("minishell: cd: OLDPWD not set", 2);
            info->exit_status = 1;
            return;
        }
        ft_putendl_fd(dir, STDOUT_FILENO);
    }
    else
    {
		printf("ja tyt xa-xa\n");
        dir = args[1];
    }

    // Смена каталога
    if (chdir(dir) != 0)
    {
        print_error(dir);
        info->exit_status = 1;
        return;
    }

    // Обновление OLDPWD
    if (update_oldpwd(info, cwd) == -1)
        return;

    // Получение нового текущего каталога
    if (!getcwd(cwd, sizeof(cwd)))
    {
        perror("minishell: getcwd");
        info->exit_status = 1;
        return;
    }

if (set_env(info, "PWD", cwd) == -1)
{
    ft_putstr_fd("minishell: cd: PWD: cannot set\n", 2);
    info->exit_status = 1;
    return;
}
    info->exit_status = 0;
}
