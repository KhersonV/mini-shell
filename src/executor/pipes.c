/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipes.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/12 13:10:51 by vmamoten          #+#    #+#             */
/*   Updated: 2025/01/09 14:52:32 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

void	child_execute(t_exec_command *current, t_info *info, int **pipes,
		t_pipeline_params *params)
{
	char	*path;

	reset_signals_to_default();
	if (params->index > 0)
		dup2(pipes[params->index - 1][0], STDIN_FILENO);
	if (params->index < params->num_cmds - 1)
		dup2(pipes[params->index][1], STDOUT_FILENO);
	free_pipes(pipes, params->num_cmds);
	if (!handle_redirections(current->redirects))
		exit(EXIT_FAILURE);
	if (is_builtin(current->cmd_name))
		exit(execute_builtin_in_child(current, info));
	path = find_command(current->cmd_name, info->envp);
	if (!path)
		exit(127);
	execve(path, current->args, info->envp);
	perror("execve");
	exit(EXIT_FAILURE);
}

int	fork_and_execute(t_exec_command *current, t_info *info, int **pipes,
		t_pipeline_params *params)
{
	pid_t	pid;

	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		free_pipes(pipes, params->num_cmds);
		return (-1);
	}
	if (pid == 0)
		child_execute(current, info, pipes, params);
	else if (params->index > 0)
		close(pipes[params->index - 1][0]);
	if (params->index < params->num_cmds - 1)
		close(pipes[params->index][1]);
	return (pid);
}

void	wait_for_children(t_info *info, pid_t last_pid)
{
	int	status;

	waitpid(last_pid, &status, 0);
	if (WIFEXITED(status))
		info->exit_status = WEXITSTATUS(status);
	else if (WIFSIGNALED(status))
		info->exit_status = 128 + WTERMSIG(status);
	while (wait(NULL) > 0)
		;
}

void	execute_pipeline(t_exec_command *commands, t_info *info)
{
	t_exec_command		*current;
	t_pipeline_params	params;
	int					**pipes;

	params.num_cmds = count_commands(commands);
	if (params.num_cmds <= 0)
		return ;
	pipes = init_pipes(params.num_cmds);
	if (!pipes)
	{
		info->exit_status = 1;
		return ;
	}
	params.index = 0;
	current = commands;
	while (current)
	{
		params.last_pid = fork_and_execute(current, info, pipes, &params);
		params.index++;
		current = current->next_cmd;
	}
	free_pipes(pipes, params.num_cmds);
	wait_for_children(info, params.last_pid);
}

int	create_heredoc_file(const char *heredoc_marker)
{
	char	*filename;
	int		fd;
	char	*line;

	filename = "/tmp/minishell_heredoc";
	fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd == -1)
	{
		perror("open");
		return (-1);
	}
	while (1)
	{
		line = readline("> ");
		if (!line || ft_strcmp(line, heredoc_marker) == 0)
		{
			free(line);
			break ;
		}
		write(fd, line, ft_strlen(line));
		write(fd, "\n", 1);
		free(line);
	}
	close(fd);
	return (open(filename, O_RDONLY));
}
