/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/10 14:25:20 by vmamoten          #+#    #+#             */
/*   Updated: 2025/01/03 13:36:15 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

int	is_n_flag(const char *arg)
{
	int	i;

	i = 1;
	if (arg[0] != '-')
		return (0);
	if (arg[1] == '\0')
		return (0);
	while (arg[i])
	{
		if (arg[i] != 'n')
			return (0);
		i++;
	}
	return (1);
}

#include <stdio.h>

static void print_escaped_backslashes(int count)
{
    // Печатаем count/2 штук '\'
    for (int i = 0; i < (count / 2); i++)
        putchar('\\');

    // Если осталось нечетное количество
    if (count % 2 == 1)
        putchar('\\');
}

void print_with_escape(const char *str)
{
    while (*str)
    {
        // Если мы нашли серию слэшей:
        if (*str == '\\')
        {
            // Подсчитываем, сколько подряд '\' идёт
            int backslash_count = 0;

            while (*str == '\\')
            {
                backslash_count++;
                str++;
            }
            // Тут серия слэшей закончилась, либо строка, либо другой символ

            // Если мы уже дошли до конца строки,
            // то выводим их согласно обычному правилу (каждые 2 -> один \)
            // и если нечетно, то последний лишний просто будет \.
            if (*str == '\0')
            {
                print_escaped_backslashes(backslash_count);
                // Выходим из цикла, т.к. строка кончилась
                break;
            }
            // Если следующий символ - это '$'
            else if (*str == '$')
            {
                // Снова выводим (backslash_count / 2) слэшей
                // а если нечетный, значит экранируем '$'
                int pair_count = backslash_count / 2;
                int odd = backslash_count % 2;

                // Печатаем попарные
                for (int i = 0; i < pair_count; i++)
                    putchar('\\');

                if (odd == 1)
                {
                    // Печатаем "\$"
                    putchar('\\');
                    putchar('$');
                }
                else
                {
                    // Печатаем просто '$'
                    putchar('$');
                }
                // Не забываем сдвинуться дальше, т.к. мы уже обработали символ `$`
                str++;
            }
            else
            {
                // Иначе, следующий символ – не `$` и не конец строки,
                // значит надо вывести (backslash_count / 2) слэшей
                // и если нечетное число, то один слэш экранирует следующий символ
                int pair_count = backslash_count / 2;
                int odd = backslash_count % 2;

                for (int i = 0; i < pair_count; i++)
                    putchar('\\');

                // Если остался лишний слэш, то экранируем следующий символ
                // (т.е. фактически печатаем \ и затем сам символ)
                if (odd == 1)
                {
                    putchar('\\');
                    // Печатаем текущий символ *str
                    putchar(*str);
                    // Сдвигаемся на следующий символ в строке
                    str++;
                }
                else
                {
                    // Если лишнего слэша нет, просто пока ничего не выводим
                    // (выведем сам символ в общем потоке ниже)
                    // НО! здесь важно не забыть, что текущий символ мы всё равно печатаем вручную
                    // Печатаем этот символ и сдвигаем str
                    putchar(*str);
                    str++;
                }
            }
        }
        else if (*str == '$')
        {
            // Если встретился '$' без всяких предшествующих слэшей
            putchar('$');
            str++;
        }
        else
        {
            // Любой другой символ без слэшей
            putchar(*str);
            str++;
        }
    }
}

void	ft_echo(t_exec_command *command, t_info *info)
{
	int	i;
	int	newline;

	i = 1;
	newline = 1;
	while (command->args[i] && is_n_flag(command->args[i]))
	{
		newline = 0;
		i++;
	}
	while (command->args[i])
	{
		print_with_escape(command->args[i]);
		if (command->args[i + 1])
			putchar(' ');
		i++;
	}
	if (newline)
		putchar('\n');
	info->exit_status = 0;
}
