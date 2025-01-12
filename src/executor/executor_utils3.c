/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_utils3.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/07 16:15:59 by vmamoten          #+#    #+#             */
/*   Updated: 2025/01/12 18:45:56 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

int	handle_empty_command(t_exec_command *command, t_info *info, int saved_stdin,
		int saved_stdout)
{
	if (!command->cmd_name || ft_strlen(command->cmd_name) == 0)
	{
		if (!handle_redirections(command->redirects))
		{
			restore_standard_fds(saved_stdin, saved_stdout);
			info->exit_status = 1;
			return (1);
		}
		restore_standard_fds(saved_stdin, saved_stdout);
		info->exit_status = 0;
		return (1);
	}
	return (0);
}

int	handle_special_cases(t_exec_command *command, t_info *info, int saved_stdin,
		int saved_stdout)
{
	if (ft_strcmp(command->cmd_name, ".") == 0)
	{
		if (!command->args[1])
		{
			ft_putendl_fd("minishell: .: filename argument required",
				STDERR_FILENO);
			ft_putendl_fd(".: usage: . filename [arguments]", STDERR_FILENO);
			info->exit_status = 2;
			restore_standard_fds(saved_stdin, saved_stdout);
			return (1);
		}
	}
	else if (ft_strcmp(command->cmd_name, "..") == 0)
	{
		ft_putendl_fd("minishell: ..: command not found", STDERR_FILENO);
		info->exit_status = 127;
		restore_standard_fds(saved_stdin, saved_stdout);
		return (1);
	}
	return (0);
}

int	handle_directory_command(t_exec_command *command, t_info *info,
		int saved_stdin, int saved_stdout)
{
	struct stat	statbuf;

	if (stat(command->cmd_name, &statbuf) == 0 && S_ISDIR(statbuf.st_mode))
	{
		ft_putendl_fd("minishell: /: is a directory", STDERR_FILENO);
		info->exit_status = 126;
		restore_standard_fds(saved_stdin, saved_stdout);
		return (1);
	}
	return (0);
}

void	ext_cmd(t_exec_command *command, t_info *info, int stin, int stout)
{
	char	*path;
	pid_t	pid;

	path = find_command(command->cmd_name, info->envp);
	if (!path)
	{
		info->exit_status = 127;
		restore_standard_fds(stin, stout);
		return ;
	}
	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		free(path);
		restore_standard_fds(stin, stout);
		return ;
	}


	if (pid == 0)
		handle_child_process(path, command, info);
	else
	{
		handle_parent_process(pid, info);
		// if(path)
		// 	free(path);
	}
}
