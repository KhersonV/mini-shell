/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: snazarov <snazarov@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/12 12:32:15 by vmamoten          #+#    #+#             */
/*   Updated: 2025/01/12 14:43:25 by snazarov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

void	execute_commands(t_exec_command *commands, t_info *info)
{
	if (commands->cmd_name && ft_strlen(commands->cmd_name) == 0)
	{
		ft_putstr_fd("minishell: : No such file or directory\n", STDERR_FILENO);
		info->exit_status = 127;
		return ;
	}
	if (!commands)
		return ;
	if (!prepare_heredocs(commands))
	{
		info->exit_status = 1;
		return ;
	}
	if (commands->next_cmd)
		execute_pipeline(commands, info);
	else
		execute_single_command(commands, info);
}

void	execute_builtin_command(t_exec_command *command, t_info *info)
{
	if (ft_strcmp(command->cmd_name, "echo") == 0)
		ft_echo(command, info);
	else if (ft_strcmp(command->cmd_name, "cd") == 0)
		ft_cd(command->args, info);
	else if (ft_strcmp(command->cmd_name, "pwd") == 0)
		ft_pwd(info);
	else if (ft_strcmp(command->cmd_name, "export") == 0)
		ft_export(command->args, info);
	else if (ft_strcmp(command->cmd_name, "env") == 0
		|| ft_strcmp(command->cmd_name, "ENV") == 0)
		ft_env(command, info);
	else if (ft_strcmp(command->cmd_name, "unset") == 0)
		unset_env(command, info);
	else if (ft_strcmp(command->cmd_name, "exit") == 0)
		ft_exit(command->args, info, command);
}

int	execute_builtin_in_child(t_exec_command *command, t_info *info)
{
	execute_builtin_command(command, info);
	return (info->exit_status);
}

void	execute_builtin(t_exec_command *command, t_info *info)
{
	int	saved_stdout;
	int	saved_stdin;

	saved_stdout = dup(STDOUT_FILENO);
	saved_stdin = dup(STDIN_FILENO);
	if (!handle_redirections(command->redirects))
	{
		restore_standard_fds(saved_stdin, saved_stdout);
		info->exit_status = 1;
		return ;
	}
	execute_builtin_command(command, info);
	restore_standard_fds(saved_stdin, saved_stdout);
}

void	execute_single_command(t_exec_command *command, t_info *info)
{
	int	saved_stdout;
	int	saved_stdin;

	saved_stdout = dup(STDOUT_FILENO);
	saved_stdin = dup(STDIN_FILENO);
	if (handle_empty_command(command, info, saved_stdin, saved_stdout))
		return ;
	if (handle_special_cases(command, info, saved_stdin, saved_stdout))
		return ;
	if (!handle_redirections(command->redirects))
	{
		restore_standard_fds(saved_stdin, saved_stdout);
		info->exit_status = 1;
		return ;
	}
	if (is_builtin(command->cmd_name))
	{
		execute_builtin(command, info);
		restore_standard_fds(saved_stdin, saved_stdout);
		return ;
	}
	if (handle_directory_command(command, info, saved_stdin, saved_stdout))
		return ;
	ext_cmd(command, info, saved_stdin, saved_stdout);
	restore_standard_fds(saved_stdin, saved_stdout);
}
