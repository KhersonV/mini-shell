/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipes.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/12 13:10:51 by vmamoten          #+#    #+#             */
/*   Updated: 2025/01/07 13:51:52 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

int	count_commands(t_exec_command *commands)
{
	int	count;

	count = 0;
	while (commands)
	{
		count++;
		commands = commands->next_cmd;
	}
	return (count);
}

int	**init_pipes(int num_cmds)
{
	int	**pipes;
	int	i;

	pipes = ft_calloc(num_cmds - 1, sizeof(int *));
	if (!pipes)
		return (NULL);
	i = 0;
	while (i < num_cmds - 1)
	{
		pipes[i] = ft_calloc(2, sizeof(int));
		if (!pipes[i] || pipe(pipes[i]) == -1)
			return (NULL);
		i++;
	}
	return (pipes);
}

void	free_pipes(int **pipes, int num_cmds)
{
	int	i;

	i = 0;
	while (i < num_cmds - 1)
	{
		if (pipes[i])
		{
			close(pipes[i][0]);
			close(pipes[i][1]);
			free(pipes[i]);
		}
		i++;
	}
	free(pipes);
}

void	child_execute(t_exec_command *current, t_info *info, int **pipes,
		t_pipeline_params *params)
{
	char	*path;

	reset_signals_to_default();
	if (params->index > 0)
		dup2(pipes[params->index  - 1][0], STDIN_FILENO);
	if (params->index < params->num_cmds - 1)
		dup2(pipes[params->index ][1], STDOUT_FILENO);
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
	else
		if (params->index> 0)
				close(pipes[params->index - 1][0]);
			if (params->index < params->num_cmds - 1)
				close(pipes[params->index][1]);
	return (pid);
}

void	execute_pipeline(t_exec_command *commands, t_info *info)
{
	t_exec_command		*current;
	t_pipeline_params	params;
	int					**pipes;
	int					status;

	params.num_cmds = count_commands(commands);
	if (params.num_cmds <= 0)
		return;
	pipes = init_pipes(params.num_cmds);
	if (!pipes)
	{
		info->exit_status = 1;
		return;
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
	waitpid(params.last_pid, &status, 0);
	if (WIFEXITED(status))
		info->exit_status = WEXITSTATUS(status);
	else if (WIFSIGNALED(status))
		info->exit_status = 128 + WTERMSIG(status);
	while (wait(NULL) > 0)
		;
}
