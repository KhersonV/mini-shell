/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit_builtin.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/10 14:25:26 by vmamoten          #+#    #+#             */
/*   Updated: 2025/01/04 11:46:20 by vmamoten         ###   ########.fr       */
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

void	ft_exit(char **args, t_info *info)
{
    int	exit_code = info->exit_status;
    int	arg_count = 0;
    int	is_interactive = isatty(STDIN_FILENO);

    while (args[arg_count])
        arg_count++;

    if (is_interactive)
        ft_putendl_fd("exit", STDOUT_FILENO);

    if (arg_count > 1)
    {
        if (!is_numeric(args[1]))
        {
            ft_putstr_fd("minishell: exit: ", STDERR_FILENO);
            ft_putstr_fd(args[1], STDERR_FILENO);
            ft_putendl_fd(": numeric argument required", STDERR_FILENO);
            exit_code = 255;
        }
        else if (arg_count > 2)
        {
            ft_putendl_fd("minishell: exit: too many arguments", STDERR_FILENO);
            info->exit_status = 1;
            return;
        }
        else
            exit_code = ft_atoi(args[1]);
    }
    free_env(info);
    exit(exit_code);
}
