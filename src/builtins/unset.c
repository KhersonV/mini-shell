/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/10 14:25:35 by vmamoten          #+#    #+#             */
/*   Updated: 2025/01/04 15:54:29 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

// Функция для обработки ошибок вывода команды unset
static void print_unset_error(char *arg)
{
    if (arg[0] == '-')
    {
        ft_putstr_fd("minishell: unset: -", STDERR_FILENO);
        ft_putchar_fd(arg[1], STDERR_FILENO); // Первый символ флага после '-'
        ft_putstr_fd(": invalid option\n", STDERR_FILENO);
        ft_putendl_fd("unset: usage: unset [-f] [-v] [name ...]", STDERR_FILENO);
    }
    else
    {
        ft_putstr_fd("minishell: unset: `", STDERR_FILENO);
        ft_putstr_fd(arg, STDERR_FILENO);
        ft_putstr_fd("': not a valid identifier\n", STDERR_FILENO);
    }
}

void unset_env(t_exec_command *command, t_info *info)
{
    int i;
    int j;

    if (!info || !command || !command->args)
        return;
    info->exit_status = 0;

    i = 1;
    while (command->args[i])
    {
        // Проверка на флаг (аргумент, начинающийся с '-')
        if (command->args[i][0] == '-')
        {
            print_unset_error(command->args[i]);
            info->exit_status = 2; // Как в Bash для неправильного флага
            return;               // Прерываем выполнение команды
        }

        // Проверка на валидность имени переменной
        if (!is_valid_env_key(command->args[i]))
        {
            print_unset_error(command->args[i]);
            info->exit_status = 1; // Как в Bash для недопустимого идентификатора
            i++;
            continue;
        }

        // Удаление переменной из окружения
        j = 0;
        while (info->envp[j])
        {
            if (env_key_compare(info->envp[j], command->args[i]))
            {
                info->envp = remove_env_entry(info->envp, j);
                break;
            }
            j++;
        }
        i++;
    }
}




	// int k = 0;
	// while (command->args[k])
	// {
	// 	printf("args n %d = %s\n", k, command->args[k]);
	// 	k++;
	// }
