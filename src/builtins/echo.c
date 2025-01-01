/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/10 14:25:20 by vmamoten          #+#    #+#             */
/*   Updated: 2025/01/01 14:04:59 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

static void	print_with_escape(const char *str)
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
	while (command->args[i] && ft_strcmp(command->args[i], "-n") == 0)
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
