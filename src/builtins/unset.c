/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: snazarov <snazarov@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/10 14:25:35 by vmamoten          #+#    #+#             */
/*   Updated: 2025/01/12 15:57:40 by snazarov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

static void	print_unset_error(char *arg, int is_option)
{
	if (is_option)
	{
		ft_putstr_fd("minishell: unset: -", STDERR_FILENO);
		ft_putchar_fd(arg[1], STDERR_FILENO);
		ft_putstr_fd(": invalid option\n", STDERR_FILENO);
		ft_putendl_fd("unset: usage: unset [-f] [-v] [name ...]",
			STDERR_FILENO);
	}
	else
	{
		ft_putstr_fd("minishell: unset: `", STDERR_FILENO);
		ft_putstr_fd(arg, STDERR_FILENO);
		ft_putendl_fd("': not a valid identifier", STDERR_FILENO);
	}
}

void	process_unset_argument(char *arg, t_info *info)
{
	int	j;

	j = 0;
	while (info->envp[j])
	{
		if (env_key_compare(info->envp[j], arg))
		{
			info->envp = remove_env_entry(info->envp, j);
			break ;
		}
		j++;
	}
}

void	unset_env(t_exec_command *command, t_info *info)
{
	int	i;

	if (!info || !command || !command->args)
		return ;
	info->exit_status = 0;
	i = 1;
	while (command->args[i])
	{
		if (command->args[i][0] == '-')
		{
			print_unset_error(command->args[i], 1);
			info->exit_status = 2;
			return ;
		}
		if (!is_valid_env_key(command->args[i]))
		{
			print_unset_error(command->args[i], 0);
			info->exit_status = 1;
			i++;
			continue ;
		}
		process_unset_argument(command->args[i], info);
		i++;
	}
}

void	exit_shell(t_info *info)
{
	free_env(info);
	free(info->old_dir);
	free(info->curr_dir);
	free(info->home);
	exit(info->exit_status);
}
