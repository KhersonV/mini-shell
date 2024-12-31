/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit_builtin.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/10 14:25:26 by vmamoten          #+#    #+#             */
/*   Updated: 2024/12/31 15:42:16 by vmamoten         ###   ########.fr       */
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
    int	exit_code = 0;
    int	arg_count = 0;

    while (args[arg_count])
        arg_count++;

    printf("exit\n");

    if (arg_count > 2)
    {
        write(2, "minishell: exit: too many arguments\n", 36);
        info->exit_status = 1;
        return ;
    }
    if (arg_count == 2)
    {
        if (!is_numeric(args[1]))
        {
            write(2, "exit: ", 6);
            write(2, args[1], ft_strlen(args[1]));
            write(2, ": numeric argument required\n", 28);
            exit_code = 255;
        }
        else
            exit_code = ft_atoi(args[1]);
    }
    free_env(info);
    exit(exit_code);
}
