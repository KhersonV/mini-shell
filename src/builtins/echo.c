/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/10 14:25:20 by vmamoten          #+#    #+#             */
/*   Updated: 2025/01/04 13:52:45 by vmamoten         ###   ########.fr       */
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

void print_with_escape(const char *str)
{
    while (*str)
    {
        // Если это бэкслэш или доллар,
        // то перед ним выведем ещё один бэкслэш
        

        // Выводим сам символ (либо обычный, либо уже "экранированный")
        putchar(*str);
        str++;
    }
}

void	ft_echo(t_exec_command *command, t_info *info)
{
	int	i;
	int	newline;
// int k = 0;

	i = 1;
	newline = 1;
	// while (command->args[k])
	// {
	// 	printf("args n %d = %s\n", k, command->args[k]);
	// 	k++;
	// }
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
