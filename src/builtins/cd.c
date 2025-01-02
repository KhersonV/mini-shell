/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/10 14:25:04 by vmamoten          #+#    #+#             */
/*   Updated: 2024/12/22 13:41:06 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

void ft_cd(char **args, t_info *info)
{
    char *dir;
    char cwd[PATH_MAX];
    char *home;

    // Обработка команды cd без аргументов или cd ~
    if (!args[1] || strcmp(args[1], "~") == 0)
    {
        home = get_env_value(info, "HOME");
        if (!home)
        {
            printf("minishell: cd: HOME not set\n");
            info->exit_status = 1;
            return;
        }
        dir = home;
    }
    else if (strcmp(args[1], "-") == 0) // Обработка команды cd -
    {
        dir = get_env_value(info, "OLDPWD");
        if (!dir)
        {
            printf("minishell: cd: OLDPWD not set\n");
            info->exit_status = 1;
            return;
        }
        printf("%s\n", dir);
    }
    else
    {
        dir = args[1];
    }

    // Получение текущего каталога
    if (!getcwd(cwd, sizeof(cwd)))
    {
        perror("minishell: getcwd");
        info->exit_status = 1;
        return;
    }

    // Смена каталога
    if (chdir(dir) != 0)
    {
        perror("minishell: cd");
        info->exit_status = 1;
        return;
    }

    // Обновление OLDPWD
    if (set_env(info, "OLDPWD", cwd) == -1)
    {
        printf("minishell: cd: failed to set OLDPWD\n");
        info->exit_status = 1;
        return;
    }

    // Получение нового текущего каталога
    if (!getcwd(cwd, sizeof(cwd)))
    {
        perror("minishell: getcwd");
        info->exit_status = 1;
        return;
    }

    // Обновление PWD
    if (set_env(info, "PWD", cwd) == -1)
    {
        printf("minishell: cd: failed to set PWD\n");
        info->exit_status = 1;
        return;
    }

    info->exit_status = 0;
}
