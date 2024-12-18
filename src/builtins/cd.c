/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/10 14:25:04 by vmamoten          #+#    #+#             */
/*   Updated: 2024/12/18 12:08:08 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

void ft_cd(char **args, t_info *info)
{
    char *dir;
    char cwd[PATH_MAX];
    char *old_pwd;
    char *home;
    char new_dir[PATH_MAX];
    char *pwd_value;

    if (!args[1] || strcmp(args[1], "~") == 0) // Если аргумент отсутствует или ~
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
    else if (strcmp(args[1], "-") == 0) // Если аргумент "-"
    {
        old_pwd = get_env_value(info, "OLDPWD");
        if (!old_pwd)
        {
            printf("minishell: cd: OLDPWD not set\n");
            info->exit_status = 1;
            return;
        }
        dir = old_pwd;
        printf("%s\n", dir); // Печатает OLDPWD
    }
    else if (args[1][0] == '~') // Если аргумент начинается с ~
    {
        home = get_env_value(info, "HOME");
        if (!home)
        {
            printf("minishell: cd: HOME not set\n");
            info->exit_status = 1;
            return;
        }
        strncpy(new_dir, home, PATH_MAX);
        strncat(new_dir, args[1] + 1, PATH_MAX - strlen(home) - 1);
        dir = new_dir;
    }
    else
        dir = args[1]; // Если передан путь

    if (!getcwd(cwd, sizeof(cwd))) // Получение текущего каталога
    {
        perror("minishell: getcwd");
        info->exit_status = 1;
        return;
    }

    if (chdir(dir) != 0) // Смена каталога
    {
        perror("minishell: cd");
        info->exit_status = 1;
        return;
    }

    pwd_value = get_env_value(info, "PWD");
    if (pwd_value)
    {
        if (set_env(info, "OLDPWD", pwd_value) == -1) // Установка OLDPWD
        {
            printf("minishell: cd: failed to set OLDPWD\n");
            info->exit_status = 1;
            return;
        }
    }
    else
    {
        if (set_env(info, "OLDPWD", cwd) == -1) // Установка OLDPWD
        {
            printf("minishell: cd: failed to set OLDPWD\n");
            info->exit_status = 1;
            return;
        }
    }

    if (!getcwd(cwd, sizeof(cwd))) // Получение нового каталога
    {
        perror("minishell: getcwd");
        info->exit_status = 1;
        return;
    }

    if (set_env(info, "PWD", cwd) == -1) // Установка PWD
    {
        printf("minishell: cd: failed to set PWD\n");
        info->exit_status = 1;
        return;
    }
    info->exit_status = 0; // Успешное завершение
}

