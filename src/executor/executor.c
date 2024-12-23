/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/12 12:32:15 by vmamoten          #+#    #+#             */
/*   Updated: 2024/12/23 14:09:45 by vmamoten         ###   ########.fr       */
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

    // Сохраняем стандартные файловые дескрипторы
    int saved_stdout = dup(STDOUT_FILENO);
    int saved_stdin = dup(STDIN_FILENO);

    // Если команда является встроенной, выполняем её
    if (is_builtin(command->cmd_name))
    {
        // Обработка перенаправлений
        if (!handle_redirections(command->redirects))
        {
            restore_standard_fds(saved_stdin, saved_stdout);
            info->exit_status = 1;
            return;
        }

        execute_builtin(command, info);
        restore_standard_fds(saved_stdin, saved_stdout); // Восстанавливаем дескрипторы
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
        // Обработка перенаправлений
        if (!handle_redirections(command->redirects))
            exit(EXIT_FAILURE);

        execve(path, command->args, info->envp); // Выполнение команды
        perror("execve");
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

    // Восстанавливаем стандартные файловые дескрипторы
    restore_standard_fds(saved_stdin, saved_stdout);
}


void execute_pipeline(t_exec_command *commands, t_info *info)
{
    int fd[2];
    int prev_fd = -1; // Файлдескриптор для предыдущей команды
    pid_t pid;

    while (commands)
    {
        if (commands->next_cmd && pipe(fd) == -1) // Создание пайпа
        {
            perror("pipe");
            return;
        }

        pid = fork();
        if (pid == -1)
        {
            perror("fork");
            return;
        }

        if (pid == 0) // Дочерний процесс
        {
            if (prev_fd != -1) // Если есть предыдущий пайп
            {
                dup2(prev_fd, STDIN_FILENO); // Подключаем ввод к предыдущему пайпу
                close(prev_fd);
            }

            if (commands->next_cmd) // Если есть следующая команда
            {
                dup2(fd[1], STDOUT_FILENO); // Подключаем вывод к текущему пайпу
                close(fd[1]);
            }

            close(fd[0]); // Закрываем чтение текущего пайпа в дочернем процессе

            if (!handle_redirections(commands->redirects))
            {
                perror("handle_redirections");
                exit(EXIT_FAILURE);
            }

            char *path = find_command(commands->cmd_name, info->envp);
            if (!path)
            {
                fprintf(stderr, "minishell: %s: command not found\n", commands->cmd_name);
                exit(127);
            }

            execve(path, commands->args, info->envp); // Выполнение команды
            perror("execve");
            exit(EXIT_FAILURE);
        }
        else // Родительский процесс
        {
            if (prev_fd != -1) // Закрываем предыдущий пайп
                close(prev_fd);

            if (commands->next_cmd) // Закрываем конец для записи текущего пайпа
                close(fd[1]);

            prev_fd = fd[0]; // Сохраняем конец для чтения текущего пайпа
            commands = commands->next_cmd;
        }
    }

    // Ожидание завершения всех дочерних процессов
    while (wait(NULL) > 0)
        ;

    // Чтение из последнего пайпа (ожидание ввода для cat)
    if (prev_fd != -1)
    {
        char buffer[1024];
        ssize_t bytes_read;
        while ((bytes_read = read(prev_fd, buffer, sizeof(buffer) - 1)) > 0)
        {
            buffer[bytes_read] = '\0';
            printf("%s", buffer);
        }
        close(prev_fd);
    }
}

void execute_builtin(t_exec_command *command, t_info *info)
{
    // Сохраняем стандартные файловые дескрипторы
    int saved_stdout = dup(STDOUT_FILENO);
    int saved_stdin = dup(STDIN_FILENO);

    // Обработка перенаправлений
    if (!handle_redirections(command->redirects))
    {
        restore_standard_fds(saved_stdin, saved_stdout);
        info->exit_status = 1;
        return;
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
        ft_env(info->envp, info);
    else if (strcmp(command->cmd_name, "exit") == 0)
        ft_exit(command->args, info);

    // Восстанавливаем стандартные файловые дескрипторы
    restore_standard_fds(saved_stdin, saved_stdout);
}
