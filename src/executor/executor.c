/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/12 12:32:15 by vmamoten          #+#    #+#             */
/*   Updated: 2025/01/07 13:01:46 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

void	execute_commands(t_exec_command *commands, t_info *info)
{
	if (commands->cmd_name && ft_strlen(commands->cmd_name) == 0)
{
    ft_putstr_fd("minishell: : No such file or directory\n", STDERR_FILENO);
    info->exit_status = 127;
    return;
}
	if (!commands)
		return ;
	if (!prepare_heredocs(commands))
	{
		info->exit_status = 1; 
		return;
	}
	if (commands->next_cmd)
		execute_pipeline(commands, info);
	else
		execute_single_command(commands, info);
}

void	execute_single_command(t_exec_command *command, t_info *info)
{
	pid_t	pid;
	int		status;
	char	*path;
	int		saved_stdout;
	int		saved_stdin;
	struct stat statbuf;

	saved_stdout = dup(STDOUT_FILENO);
	saved_stdin = dup(STDIN_FILENO);

	if (!command->cmd_name || ft_strlen(command->cmd_name) == 0)
	{
		if (!handle_redirections(command->redirects))
		{
			restore_standard_fds(saved_stdin, saved_stdout);
			info->exit_status = 1;
			return;
		}
		restore_standard_fds(saved_stdin, saved_stdout);
		info->exit_status = 0;
		return;
	}

	if (ft_strcmp(command->cmd_name, ".") == 0)
	{
		if (!command->args[1])
		{
			ft_putendl_fd("minishell: .: filename argument required", STDERR_FILENO);
			ft_putendl_fd(".: usage: . filename [arguments]", STDERR_FILENO);
			info->exit_status = 2;
			restore_standard_fds(saved_stdin, saved_stdout);
			return;
		}
	}
	else if (ft_strcmp(command->cmd_name, "..") == 0)
	{
		ft_putendl_fd("minishell: ..: command not found", STDERR_FILENO);
		info->exit_status = 127;
		restore_standard_fds(saved_stdin, saved_stdout);
		return;
	}

	if (!handle_redirections(command->redirects))
	{
		restore_standard_fds(saved_stdin, saved_stdout);
		info->exit_status = 1;
		return;
	}

	if (is_builtin(command->cmd_name))
	{
		execute_builtin(command, info);
		restore_standard_fds(saved_stdin, saved_stdout);
		return;
	}

	if (stat(command->cmd_name, &statbuf) == 0 && S_ISDIR(statbuf.st_mode))
	{
		ft_putendl_fd("minishell: /: is a directory", STDERR_FILENO);
		info->exit_status = 126; 
		restore_standard_fds(saved_stdin, saved_stdout);
		return;
	}

	path = find_command(command->cmd_name, info->envp);
	if (!path)
	{
		info->exit_status = 127; 
		restore_standard_fds(saved_stdin, saved_stdout);
		return;
	}

	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		free(path);
		restore_standard_fds(saved_stdin, saved_stdout);
		return;
	}
	if (pid == 0)
	{
		reset_signals_to_default();
		execve(path, command->args, info->envp);
		perror("execve");
		free(path);
		exit(EXIT_FAILURE);
	}
	else
	{
		free(path);
		waitpid(pid, &status, 0);
		if (WIFEXITED(status))
			info->exit_status = WEXITSTATUS(status);
		else if (WIFSIGNALED(status))
			info->exit_status = 128 + WTERMSIG(status);
	}

	restore_standard_fds(saved_stdin, saved_stdout);
}

static void	execute_builtin_command(t_exec_command *command, t_info *info)
{
	if (ft_strcmp(command->cmd_name, "echo") == 0)
		ft_echo(command, info);
	else if (ft_strcmp(command->cmd_name, "cd") == 0)
		ft_cd(command->args, info);
	else if (ft_strcmp(command->cmd_name, "pwd") == 0)
		ft_pwd(info);
	else if (ft_strcmp(command->cmd_name, "export") == 0)
		ft_export(command->args, info);
	else if (ft_strcmp(command->cmd_name, "env") == 0 || ft_strcmp(command->cmd_name, "ENV") == 0)
		ft_env(command, info);
	else if (ft_strcmp(command->cmd_name, "unset") == 0)
		unset_env(command, info);
	else if (ft_strcmp(command->cmd_name, "exit") == 0)
		ft_exit(command->args, info);
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
		return;
	}
	execute_builtin_command(command, info);
	restore_standard_fds(saved_stdin, saved_stdout);
}

// int	count_commands(t_exec_command *commands)
// {
// 	int	count;

// 	count = 0;
// 	while (commands)
// 	{
// 		count++;
// 		commands = commands->next_cmd;
// 	}
// 	return (count);
// }

// int	**init_pipes(int num_cmds)
// {
// 	int	**pipes;
// 	int	i;

// 	pipes = ft_calloc(num_cmds - 1, sizeof(int *));
// 	if (!pipes)
// 		return (NULL);
// 	i = 0;
// 	while (i < num_cmds - 1)
// 	{
// 		pipes[i] = ft_calloc(2, sizeof(int));
// 		if (!pipes[i || pipe(pipes[i]) == -1])
// 			return (NULL);
// 		i++;
// 	}
// 	return (pipes);
// }

// void	free_pipes(int **pipes, int num_cmds)
// {
// 	int	i;

// 	i = 0;
// 	while (i < num_cmds - 1)
// 	{
// 		if (pipes[i])
// 		{
// 			close(pipes[i][0]);
// 			close(pipes[i][1]);
// 			free(pipes[i]);
// 		}
// 		i++;
// 	}
// 	free(pipes);
// }

// void	execute_pipeline(t_exec_command *commands, t_info *info)
// {
// 	int				**pipes;
// 	pid_t			pid;
// 	int				process_index;
// 	int				num_cmds;
// 	t_exec_command	*current;
// 	char			*path;
// 	int				status;

// 	num_cmds = count_commands(commands);
// 	if (num_cmds <= 0)
// 		return ;
// 	pipes = init_pipes(num_cmds);
// 	if (!pipes)
// 	{
// 		info->exit_status = 1;
// 		return ;
// 	}
// 	process_index = 0;
// 	current = commands;
// 	while (current)
// 	{
// 		pid = fork();
// 		if (pid == -1)
// 		{
// 			perror("fork");
// 			free_pipes(pipes, num_cmds);
// 			return ;
// 		}
// 		if (pid == 0)
// 		{
// 			reset_signals_to_default();
// 			if (process_index > 0)
// 				dup2(pipes[process_index - 1][0], STDIN_FILENO);
// 			if (process_index < num_cmds - 1)
// 				dup2(pipes[process_index][1], STDOUT_FILENO);
// 			free_pipes(pipes, num_cmds);
// 			if (!handle_redirections(current->redirects))
// 				exit(EXIT_FAILURE);
// 			if (is_builtin(current->cmd_name))
// 				exit(execute_builtin_in_child(current, info));
// 			else
// 			{
// 				path = find_command(current->cmd_name, info->envp);
// 				if (!path)
// 					exit(127);
// 				execve(path, current->args, info->envp);
// 				perror("execve");
// 				exit(EXIT_FAILURE);
// 			}
// 		}
// 		else
// 		{
// 			if (process_index > 0)
// 				close(pipes[process_index - 1][0]);
// 			if (process_index < num_cmds - 1)
// 				close(pipes[process_index][1]);
// 		}
// 		process_index++;
// 		current = current->next_cmd;
// 	}
// 	free_pipes(pipes, num_cmds);
// 	waitpid(pid, &status, 0);
// 	if (WIFEXITED(status))
// 		info->exit_status = WEXITSTATUS(status);
// 	else if (WIFSIGNALED(status))
// 		info->exit_status = 128 + WTERMSIG(status);
// 	while (wait(NULL) > 0)
// 		;
// }
