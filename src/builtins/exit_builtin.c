/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit_builtin.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/10 14:25:26 by vmamoten          #+#    #+#             */
/*   Updated: 2025/01/15 13:41:40 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

int	is_numeric(const char *str)
{
	if (*str == '-' || *str == '+')
		str++;
	if (!*str)
		return (0);
	while (*str)
	{
		if (!ft_isdigit(*str))
			return (0);
		str++;
	}
	return (1);
}

int	handle_numeric_argument_error(char *arg)
{
	ft_putstr_fd("minishell: exit: ", STDERR_FILENO);
	ft_putstr_fd(arg, STDERR_FILENO);
	ft_putendl_fd(": numeric argument required", STDERR_FILENO);
	return (255);
}

int	process_exit_argument(const char *arg)
{
	long long	value;
	int			exit_code;

	value = ft_atoll(arg);
	if (value < 0)
		exit_code = 256 + (value % 256);
	else
		exit_code = value % 256;
	return (exit_code);
}

int	handle_exit_args(char **args, t_info *info)
{
	int	exit_code;
	int	arg_count;

	exit_code = info->exit_status;
	arg_count = 0;
	while (args[arg_count])
		arg_count++;
	if (arg_count > 1)
	{
		if (!is_numeric(args[1]))
			return (handle_numeric_argument_error(args[1]));
		if (arg_count > 2)
		{
			ft_putendl_fd("minishell: exit: too many arguments", STDERR_FILENO);
			info->exit_status = 1;
			return (-1);
		}
		exit_code = process_exit_argument(args[1]);
	}
	return (exit_code);
}

void	ft_exit(char **args, t_info *info, t_exec_command *commands)
{
	int	exit_code;
	int	is_interactive;

	is_interactive = isatty(STDIN_FILENO);
	if (is_interactive)
		ft_putendl_fd("exit", STDOUT_FILENO);
	exit_code = handle_exit_args(args, info);
	if (exit_code == -1)
		return ;
	free_info(info);
	free_array(&commands);
	exit(exit_code);
}
