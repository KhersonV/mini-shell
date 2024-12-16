/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit_builtin.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/10 14:25:26 by vmamoten          #+#    #+#             */
/*   Updated: 2024/12/16 14:19:52 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

void ft_exit(char **args, t_info *info)
{
    int exit_code = 0;

    printf("exit\n");
    if (args[1])
    {
        if (!ft_isdigit(args[1][0]))
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
