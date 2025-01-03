/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/12 12:32:15 by vmamoten          #+#    #+#             */
/*   Updated: 2025/01/03 14:59:50 by vmamoten         ###   ########.fr       */
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

void execute_single_command(t_exec_command *command, t_info *info)
{
    pid_t pid;
    int status;
    char *path;
    int saved_stdout;
    int saved_stdin;

    saved_stdout = dup(STDOUT_FILENO);
    saved_stdin = dup(STDIN_FILENO);
    if (is_builtin(command->cmd_name))
    {
        if (!handle_redirections(command->redirects))
        {
            restore_standard_fds(saved_stdin, saved_stdout);
            info->exit_status = 1;
            return;
        }
        execute_builtin(command, info);
        restore_standard_fds(saved_stdin, saved_stdout);
        return;
    }
    path = find_command(command->cmd_name, info->envp);
    if (!path)
    {
        info->exit_status = 127;
        return; // Ошибка уже обработана в find_command
    }
    pid = fork();
    if (pid == -1)
    {
        perror("fork");
        free(path);
        return;
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


// void	execute_pipeline(t_exec_command *commands, t_info *info)
// {
// 	int		fd[2];
// 	pid_t	pid;
// 	char	*path;
// 	char	buffer[1024];
// 	ssize_t	bytes_read;
// 	int		prev_fd;

// 	prev_fd = -1;
// 	while (commands)
// 	{
// 		if (commands->next_cmd && pipe(fd) == -1)
// 		{
// 			perror("pipe");
// 			return ;
// 		}

// 		if (is_builtin(commands->cmd_name) && !commands->next_cmd)
//         {
//             // Выполнение встроенной команды в основном процессе
//             if (prev_fd != -1)
//             {
//                 dup2(prev_fd, STDIN_FILENO);
//                 close(prev_fd);
//             }
//             execute_builtin(commands, info);
//             return; // Завершаем, так как встроенная команда выполнена
//         }

// 		pid = fork();
// 		if (pid == -1)
// 		{
// 			perror("fork");
// 			return ;
// 		}
// 		if (pid == 0)
// 		{
// 			if (prev_fd != -1)
// 			{
// 				dup2(prev_fd, STDIN_FILENO);
// 				close(prev_fd);
// 			}
// 			if (commands->next_cmd)
// 			{
// 				dup2(fd[1], STDOUT_FILENO);
// 				close(fd[1]);
// 			}
// 			close(fd[0]);
			

// 			if (!handle_redirections(commands->redirects))
// 			{
// 				perror("handle_redirections");
// 				exit(EXIT_FAILURE);
// 			}
// 			path = find_command(commands->cmd_name, info->envp);
// 			if (!path)
// 			{
// 				fprintf(stderr, "minishell: %s: command not found\n",
// 					commands->cmd_name);
// 				exit(127);
// 			}
// 			execve(path, commands->args, info->envp);
// 			perror("execve");
// 			exit(EXIT_FAILURE);
// 		}
// 		else
// 		{
// 			if (prev_fd != -1)
// 				close(prev_fd);
// 			if (commands->next_cmd)
// 				close(fd[1]);
// 			prev_fd = fd[0];
// 			commands = commands->next_cmd;
// 		}
// 	}
// 	while (wait(NULL) > 0)
// 		;
// 	if (prev_fd != -1)
// 	{
// 		while ((bytes_read = read(prev_fd, buffer, sizeof(buffer) - 1)) > 0)
// 		{
// 			buffer[bytes_read] = '\0';
// 			printf("%s", buffer);
// 		}
// 		close(prev_fd);
// 	}
// }

int count_commands(t_exec_command *commands)
{
    int count = 0;

    while (commands)
    {
        count++;
        commands = commands->next_cmd;
    }

    return count;
}

int **init_pipes(int num_cmds)
{
    int **pipes = ft_calloc(num_cmds - 1, sizeof(int *));
    if (!pipes)
        {//quit_program(EXIT_FAILURE);
		}

    for (int i = 0; i < num_cmds - 1; i++)
    {
        pipes[i] = ft_calloc(2, sizeof(int));
        if (!pipes[i] || pipe(pipes[i]) == -1)
		{
            //quit_program(EXIT_FAILURE);
		}
    }
    return pipes;
}

void free_pipes(int **pipes, int num_cmds)
{
    for (int i = 0; i < num_cmds - 1; i++)
    {
        if (pipes[i])
        {
            close(pipes[i][0]);
            close(pipes[i][1]);
            free(pipes[i]);
        }
    }
    free(pipes);
}

int execute_builtin_in_child(t_exec_command *command, t_info *info)
{
    if (ft_strcmp(command->cmd_name, "echo") == 0)
			ft_echo(command, info);
    else if (ft_strcmp(command->cmd_name, "cd") == 0)
        ft_cd(command->args, info);
    else if (ft_strcmp(command->cmd_name, "pwd") == 0)
        ft_pwd(info);
    else if (ft_strcmp(command->cmd_name, "export") == 0)
        ft_export(command->args, info);
    else if (ft_strcmp(command->cmd_name, "env") == 0)
        ft_env(command, info);
    else if (ft_strcmp(command->cmd_name, "unset") == 0)
        unset_env(command->args, info);
    else if (ft_strcmp(command->cmd_name, "exit") == 0)
        ft_exit(command->args, info);
    return (info->exit_status);
}

void execute_pipeline(t_exec_command *commands, t_info *info)
{
    int **pipes;
    pid_t pid;
    int process_index = 0;
    int num_cmds = count_commands(commands);
    t_exec_command *current = commands;

    // Инициализация пайпов
    pipes = init_pipes(num_cmds);

    while (current)
    {
        pid = fork();
        if (pid == -1)
        {
            perror("fork");
            free_pipes(pipes, num_cmds);
            return;
        }

        if (pid == 0) // Дочерний процесс
        {
            // Настройка входа/выхода
            if (process_index > 0)
                dup2(pipes[process_index - 1][0], STDIN_FILENO);
            if (process_index < num_cmds - 1)
                dup2(pipes[process_index][1], STDOUT_FILENO);

            free_pipes(pipes, num_cmds);

            if (!handle_redirections(current->redirects))
                exit(EXIT_FAILURE);

            // Выполняем builtin или execve
            if (is_builtin(current->cmd_name))
                exit(execute_builtin_in_child(current, info));
            else
            {
                char *path = find_command(current->cmd_name, info->envp);
                if (!path)
                    exit(127);
                execve(path, current->args, info->envp);
                perror("execve");
                exit(EXIT_FAILURE);
            }
        }
        else // Родитель
        {
            // Закрываем неиспользуемые концы
            if (process_index > 0)
                close(pipes[process_index - 1][0]);
            if (process_index < num_cmds - 1)
                close(pipes[process_index][1]);
        }

        process_index++;
        current = current->next_cmd;
    }

    // Освобождаем память под пайпы
    free_pipes(pipes, num_cmds);

    // Старый код: while (wait(NULL) > 0) ;
    // Но нам нужно отследить exit code последнего запущенного PID

    // Добавляем так:
    // Вспомним, pid каждого fork() мы записывали в переменную pid
    // - т.е. в конце цикла в pid лежит PID последнего процесса

    int status;
    // Ждем именно последний PID
    waitpid(pid, &status, 0);

    if (WIFEXITED(status))
        info->exit_status = WEXITSTATUS(status);
    else if (WIFSIGNALED(status))
        info->exit_status = 128 + WTERMSIG(status);

    // Остальных детей тоже ждем, но их коды уже не влияют на info->exit_status
    while (wait(NULL) > 0)
        ;
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
	if (ft_strcmp(command->cmd_name, "echo") == 0)
		ft_echo(command, info);
	else if (ft_strcmp(command->cmd_name, "cd") == 0)
		ft_cd(command->args, info);
	else if (ft_strcmp(command->cmd_name, "pwd") == 0)
		ft_pwd(info);
	else if (ft_strcmp(command->cmd_name, "export") == 0)
		ft_export(command->args, info);
	else if (ft_strcmp(command->cmd_name, "env") == 0)
		ft_env(command, info);
	else if (ft_strcmp(command->cmd_name, "unset") == 0)
		unset_env(command->args, info);
	else if (ft_strcmp(command->cmd_name, "exit") == 0)
		ft_exit(command->args, info);
	restore_standard_fds(saved_stdin, saved_stdout);
}
