/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/10 14:25:20 by vmamoten          #+#    #+#             */
/*   Updated: 2025/01/01 16:03:50 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

int	is_n_flag(const char *arg)
{
	if (arg[0] != '-')
		return (0);
	for (int i = 1; arg[i]; i++)
	{
		if (arg[i] != 'n')
			return (0);
	}
	return (1);
}

void	print_with_escape(const char *str)
{
	while (*str)
	{
		if (*str == '\\' && *(str + 1) != '\0')
		{
			str++;
			if (*str == 'n')
				putchar('\n');
			else if (*str == 't')
				putchar('\t');
			else if (*str == 'r')
				putchar('\r');
			else
				putchar(*str);
		}
		else
		{
			putchar(*str);
		}
		str++;
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
		if (command->args[i + 1] && command->args[i][0] != '\0')
			putchar(' ');
		i++;
	}
	if (newline)
		putchar('\n');
	info->exit_status = 0;
}
