/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_builtin.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/10 14:25:23 by vmamoten          #+#    #+#             */
/*   Updated: 2025/01/06 16:40:12 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

void	p_r(char *command)
{
	char	*prefix;
	char	*message;

	prefix = ft_strjoin("minishell: ", command);
	if (!prefix)
		return ;
	message = ft_strjoin(prefix, ": command not found\n");
	free(prefix);
	if (!message)
		return ;
	ft_putstr_fd(message, STDERR_FILENO);
	free(message);
}

void	run_env(char *path, t_exec_command *command, char **new_env)
{
	execve(path, &command->args[1], new_env);
	perror("execve");
	free(path);
	ft_free_array(new_env);
	exit(EXIT_FAILURE);
}

void	update_and_print_env(t_exec_command *command, t_info *info,
		char ***new_env)
{
	int	i;

	*new_env = env_to_array(info);
	i = 1;
	while (command->args[i] && ft_strchr(command->args[i], '='))
	{
		*new_env = append_env_entry(*new_env, command->args[i]);
		i++;
	}
	if (!command->args[i])
	{
		i = 0;
		while ((*new_env)[i])
		{
			ft_putendl_fd((*new_env)[i], STDOUT_FILENO);
			i++;
		}
		ft_free_array(*new_env);
		info->exit_status = 0;
	}
}

void	execute_command_with_env(t_exec_command *command, t_info *info,
		char **new_env)
{
	char	*path;

	path = find_command(command->args[1], new_env);
	if (!path)
	{
		info->exit_status = 127;
		p_r(command->args[1]);
		ft_free_array(new_env);
		return ;
	}
	run_env(path, command, new_env);
}

void	ft_env(t_exec_command *command, t_info *info)
{
	char	**new_env;

	update_and_print_env(command, info, &new_env);
	if (!command->args[1])
		return ;
	execute_command_with_env(command, info, new_env);
}
