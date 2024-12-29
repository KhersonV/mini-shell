/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/12 12:32:15 by vmamoten          #+#    #+#             */
/*   Updated: 2024/12/29 12:40:56 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

void	execute_commands(t_exec_command *commands, t_info *info)
{
	if (!commands)
		return ;
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

	saved_stdout = dup(STDOUT_FILENO);
	saved_stdin = dup(STDIN_FILENO);
	if (is_builtin(command->cmd_name))
	{
		if (!handle_redirections(command->redirects))
		{
			restore_standard_fds(saved_stdin, saved_stdout);
			info->exit_status = 1;
			return ;
		}
		execute_builtin(command, info);
		restore_standard_fds(saved_stdin, saved_stdout);
		return ;
	}
	path = find_command(command->cmd_name, info->envp);
	if (!path)
	{
		fprintf(stderr, "minishell: %s: command not found\n",
			command->cmd_name);
		info->exit_status = 127;
		return ;
	}
	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		free(path);
		return ;
	}
	if (pid == 0)
	{
		if (!handle_redirections(command->redirects))
			exit(EXIT_FAILURE);
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

void	execute_pipeline(t_exec_command *commands, t_info *info)
{
	int		fd[2];
	pid_t	pid;
	char	*path;
	char	buffer[1024];
	ssize_t	bytes_read;
	int		prev_fd;

	prev_fd = -1;
	while (commands)
	{
		if (commands->next_cmd && pipe(fd) == -1)
		{
			perror("pipe");
			return ;
		}
		pid = fork();
		if (pid == -1)
		{
			perror("fork");
			return ;
		}
		if (pid == 0)
		{
			if (prev_fd != -1)
			{
				dup2(prev_fd, STDIN_FILENO);
				close(prev_fd);
			}
			if (commands->next_cmd)
			{
				dup2(fd[1], STDOUT_FILENO);
				close(fd[1]);
			}
			close(fd[0]);
			if (!handle_redirections(commands->redirects))
			{
				perror("handle_redirections");
				exit(EXIT_FAILURE);
			}
			path = find_command(commands->cmd_name, info->envp);
			if (!path)
			{
				fprintf(stderr, "minishell: %s: command not found\n",
					commands->cmd_name);
				exit(127);
			}
			execve(path, commands->args, info->envp);
			perror("execve");
			exit(EXIT_FAILURE);
		}
		else
		{
			if (prev_fd != -1)
				close(prev_fd);
			if (commands->next_cmd)
				close(fd[1]);
			prev_fd = fd[0];
			commands = commands->next_cmd;
		}
	}
	while (wait(NULL) > 0)
		;
	if (prev_fd != -1)
	{
		while ((bytes_read = read(prev_fd, buffer, sizeof(buffer) - 1)) > 0)
		{
			buffer[bytes_read] = '\0';
			printf("%s", buffer);
		}
		close(prev_fd);
	}
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
	if (strcmp(command->cmd_name, "echo") == 0)
		ft_echo(command->args, info);
	else if (strcmp(command->cmd_name, "cd") == 0)
		ft_cd(command->args, info);
	else if (strcmp(command->cmd_name, "pwd") == 0)
		ft_pwd(info);
	else if (strcmp(command->cmd_name, "export") == 0)
		ft_export(command->args, info);
	else if (strcmp(command->cmd_name, "env") == 0)
		ft_env(command, info);
	else if (strcmp(command->cmd_name, "exit") == 0)
		ft_exit(command->args, info);
	restore_standard_fds(saved_stdin, saved_stdout);
}
