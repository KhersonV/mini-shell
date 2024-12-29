/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_builtin.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/10 14:25:23 by vmamoten          #+#    #+#             */
/*   Updated: 2024/12/28 14:06:35 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

void ft_env(char **args, t_info *info)
{
    char **new_env = env_to_array(info);
    int i = 1;

    // Добавляем временные переменные окружения
    while (args[i] && strchr(args[i], '='))
    {
        new_env = append_env_entry(new_env, args[i]);
        i++;
    }

    if (!args[i]) // Если команды нет, просто выводим окружение
    {
        for (int j = 0; new_env[j]; j++)
            printf("%s\n", new_env[j]);
        free_env_array(new_env);
        info->exit_status = 0;
        return;
    }

    // Выполняем указанную команду
    char *path = find_command(args[i], new_env);
    if (!path)
    {
        fprintf(stderr, "minishell: %s: command not found\n", args[i]);
        free_env_array(new_env);
        info->exit_status = 127;
        return;
    }

    execve(path, &args[i], new_env);
    perror("execve");
    free(path);
    free_env_array(new_env);
    exit(EXIT_FAILURE);
}
