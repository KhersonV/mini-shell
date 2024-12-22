/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/12 12:32:15 by vmamoten          #+#    #+#             */
/*   Updated: 2024/12/22 13:57:52 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

void	execute_commands(t_exec_command *commands, t_info *info)
{
	if (!commands)
		return ;
	if (commands->next_cmd)
	{
		execute_pipeline(commands, info);
		return;
	}
	else
		execute_single_command(commands, info);
}

void execute_single_command(t_exec_command *command, t_info *info)
{
    pid_t pid;
    int status;
    char *path;

    // Если команда является встроенной, выполняем её
    if (is_builtin(command->cmd_name))
    {
        execute_builtin(command, info);
        return;
    }

    // Поиск команды
    path = find_command(command->cmd_name, info->envp);
    if (!path)
    {
        fprintf(stderr, "minishell: %s: command not found\n", command->cmd_name);
        info->exit_status = 127;
        return;
    }

    pid = fork(); // Создаём дочерний процесс
    if (pid == -1)
    {
        perror("fork");
        free(path);
        return;
    }

    if (pid == 0) // Дочерний процесс
    {
        if (!handle_redirections(command->redirects))
            exit(EXIT_FAILURE);

        execve(path, command->args, info->envp); // Выполнение команды
        perror("execve"); // Если execve вернул управление, произошла ошибка
        free(path);
        exit(EXIT_FAILURE);
    }
    else // Родительский процесс
    {
        free(path);
        waitpid(pid, &status, 0); // Ожидание завершения дочернего процесса
        if (WIFEXITED(status))
            info->exit_status = WEXITSTATUS(status);
        else if (WIFSIGNALED(status))
            info->exit_status = 128 + WTERMSIG(status);
    }
}

void	execute_pipeline(t_exec_command *commands, t_info *info)
{
	int		fd[2];
	pid_t	pid;
	int		prev_fd;
	char	*path;

	prev_fd = -1;
	while (commands)
	{
		if (pipe(fd) == -1)
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
				dup2(fd[1], STDOUT_FILENO);
			close(fd[0]);
			close(fd[1]);
			if (!handle_redirections(commands->redirects))
				exit(EXIT_FAILURE);
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
			close(fd[1]);
			if (prev_fd != -1)
				close(prev_fd);
			prev_fd = fd[0];
			commands = commands->next_cmd;
		}
	}
	while (wait(NULL) > 0)
		;
}

void execute_builtin(t_exec_command *command, t_info *info)
{
    if (!command || !command->cmd_name)
        return;
    if (strcmp(command->cmd_name, "echo") == 0)
        ft_echo(command->args, info);
    else if (strcmp(command->cmd_name, "cd") == 0)
        ft_cd(command->args, info);
    else if (strcmp(command->cmd_name, "pwd") == 0)
        ft_pwd(info);
    else if (strcmp(command->cmd_name, "export") == 0)
        ft_export(command->args, info);
    else if (strcmp(command->cmd_name, "env") == 0)
        ft_env(info->envp, info);
    else if (strcmp(command->cmd_name, "exit") == 0)
        ft_exit(command->args, info);
}
