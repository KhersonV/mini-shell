/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/12 13:09:33 by vmamoten          #+#    #+#             */
/*   Updated: 2025/01/03 11:48:25 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

int handle_redirections(t_redirection *redirects)
{
    int fd;

    while (redirects)
    {
        if (redirects->type == TOKEN_REDIRECT_OUT) // >
            fd = open(redirects->filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        else if (redirects->type == TOKEN_REDIRECT_APPEND) // >>
            fd = open(redirects->filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
        else if (redirects->type == TOKEN_REDIRECT_IN) // <
            fd = open(redirects->filename, O_RDONLY);
        else
        {
            fprintf(stderr, "minishell: Unsupported redirection type\n");
            return 0;
        }

        if (fd == -1)
        {
            perror(redirects->filename);
            return 0;
        }

        if (dup2(fd, (redirects->type == TOKEN_REDIRECT_IN) ? STDIN_FILENO : STDOUT_FILENO) == -1)
        {
            perror("dup2");
            close(fd);
            return 0;
        }

        close(fd);
        redirects = redirects->next;
    }
    return 1;
}


void	restore_standard_fds(int fd_in, int fd_out)
{
	if (fd_in != -1)
	{
		if (dup2(fd_in, STDIN_FILENO) == -1)
			perror("dup2");
		close(fd_in);
	}
	if (fd_out != -1)
	{
		if (dup2(fd_out, STDOUT_FILENO) == -1)
			perror("dup2");
		close(fd_out);
	}
}

int	is_builtin(char *command)
{
	if (!command)
		return (0);
	return (strcmp(command, "echo") == 0 || strcmp(command, "cd") == 0
		|| strcmp(command, "pwd") == 0 || strcmp(command, "export") == 0
		|| strcmp(command, "unset") == 0 || strcmp(command, "env") == 0
		|| strcmp(command, "exit") == 0);
}

char *find_command(char *command, char **envp)
{
    struct stat statbuf; // Для проверки типа файла
    char *path_env;
    char **paths;
    char *full_path;
    char *temp;
    int i;

    // Если команда содержит "/", проверяем её как путь
    if (ft_strchr(command, '/'))
    {
        if (stat(command, &statbuf) == 0)
        {
            if (S_ISDIR(statbuf.st_mode)) // Проверяем, является ли это директорией
            {
                fprintf(stderr, "minishell: %s: is a directory\n", command);
                return (NULL);
            }
            if (access(command, X_OK) == 0) // Проверяем права на выполнение
                return (ft_strdup(command));
        }
        fprintf(stderr, "minishell: %s: command not found\n", command);
        return (NULL);
    }

    // Получение переменной PATH из окружения
    path_env = get_env_value_direct(envp, "PATH");
    if (!path_env)
        return (NULL);

    paths = ft_split(path_env, ':');
    free(path_env);
    if (!paths)
        return (NULL);

    // Проверка в каждом пути из PATH
    i = 0;
    while (paths[i])
    {
        temp = ft_strjoin(paths[i], "/");
        full_path = ft_strjoin(temp, command);
        free(temp);
        if (stat(full_path, &statbuf) == 0)
        {
            if (S_ISDIR(statbuf.st_mode)) // Если это директория, игнорируем
            {
                free(full_path);
                i++;
                continue;
            }
            if (access(full_path, X_OK) == 0) // Проверяем права на выполнение
            {
                ft_free_array(paths);
                return (full_path);
            }
        }
        free(full_path);
        i++;
    }
    ft_free_array(paths);
    fprintf(stderr, "minishell: %s: command not found\n", command);
    return (NULL); // Команда не найдена
}
