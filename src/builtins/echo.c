/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/10 14:25:20 by vmamoten          #+#    #+#             */
/*   Updated: 2024/12/22 15:36:17 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

static void print_with_escape(const char *str)
{
    while (*str)
    {
        if (*str == '\\' && *(str + 1) != '\0') // Если найден символ '\\'
        {
            str++;
            if (*str == 'n')
                putchar('\n');
            else if (*str == 't')
                putchar('\t');
            else if (*str == 'r')
                putchar('\r');
            else
                putchar(*str); // Если это не известная последовательность
        }
        else
        {
            putchar(*str);
        }
        str++;
    }
}

void ft_echo(char **args, t_info *info)
{
    int i = 1; // Начинаем с первого аргумента
    int newline = 1; // По умолчанию добавляем новую строку

    // Проверяем флаг -n
    while (args[i] && strcmp(args[i], "-n") == 0)
    {
        newline = 0;
        i++;
    }

    // Вывод аргументов с обработкой escape-последовательностей
    while (args[i])
    {
        print_with_escape(args[i]); // Обработка строки
        if (args[i + 1]) // Добавляем пробел между аргументами
            putchar(' ');
        i++;
    }

    if (newline) // Добавляем новую строку, если флаг -n не установлен
        putchar('\n');

    info->exit_status = 0; // Устанавливаем статус выполнения
}
