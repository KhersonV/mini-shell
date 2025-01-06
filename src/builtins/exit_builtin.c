/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit_builtin.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/10 14:25:26 by vmamoten          #+#    #+#             */
/*   Updated: 2025/01/06 16:38:02 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

long long	ft_atoll(const char *str)
{
	long long	result;
	int			sign;

	result = 0;
	sign = 1;
	while (*str == ' ' || (*str >= 9 && *str <= 13))
		str++;
	if (*str == '-' || *str == '+')
	{
		if (*str == '-')
			sign = -1;
		str++;
	}
	while (*str && *str >= '0' && *str <= '9')
	{
		if (result > (LLONG_MAX - (*str - '0')) / 10)
			return (sign == 1 ? LLONG_MAX : LLONG_MIN);
		result = result * 10 + (*str - '0');
		str++;
	}
	return (result * sign);
}

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

int	handle_exit_args(char **args, t_info *info)
{
	long long	value;
	int			exit_code;
	int			arg_count;

	exit_code = info->exit_status;
	arg_count = 0;
	while (args[arg_count])
		arg_count++;
	if (arg_count > 1)
	{
		if (!is_numeric(args[1]))
			return (handle_numeric_argument_error(args[1]));
		else if (arg_count > 2)
		{
			ft_putendl_fd("minishell: exit: too many arguments", STDERR_FILENO);
			info->exit_status = 1;
			return (-1);
		}
		else
		{
			value = ft_atoll(args[1]);
			exit_code = (value < 0) ? 256 + (value % 256) : value % 256;
		}
	}
	return (exit_code);
}

void	ft_exit(char **args, t_info *info)
{
	int	exit_code;
	int	is_interactive;

	is_interactive = isatty(STDIN_FILENO);
	if (is_interactive)
		ft_putendl_fd("exit", STDOUT_FILENO);
	exit_code = handle_exit_args(args, info);
	if (exit_code == -1)
		return ;
	free_env(info);
	exit(exit_code);
}
