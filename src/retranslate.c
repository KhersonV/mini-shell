/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   retranslate.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/22 11:19:30 by vmamoten          #+#    #+#             */
/*   Updated: 2024/09/25 14:13:39 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	execute_command_with_redirect(char **args, char *outfile, int append,
		char **envp)
{
	pid_t	pid;
	int		status;
	int		fd;

	pid = fork();
	if (pid == -1)
		return (perror("minishell: fork"));
	else if (pid == 0)
	{
		if (append)
			fd = open(outfile, O_WRONLY | O_CREAT | O_APPEND, 0644);
		else
			fd = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if (fd == -1)
		{
			perror("minishell: open");
			exit(1);
		}
		dup2(fd, STDOUT_FILENO);
		close(fd);
		execve(find_command(args[0]), args, envp);
		perror("minishell: execve");
		exit(1);
	}
	else
		waitpid(pid, &status, 0);
}

void	execute_pipeline(char ***cmds, char **envp)
{
	int	fd[2];

	pid_t pid1, pid2;
	if (pipe(fd) == -1)
	{
		perror("minishell: pipe");
		return ;
	}
	pid1 = fork();
	if (pid1 == -1)
	{
		perror("minishell: fork");
		return ;
	}
	if (pid1 == 0)
	{
		dup2(fd[1], STDOUT_FILENO);
		close(fd[0]);
		close(fd[1]);
		execve(find_command(cmds[0][0]), cmds[0], envp);
		perror("minishell: execve");
		exit(1);
	}
	pid2 = fork();
	if (pid2 == -1)
	{
		perror("minishell: fork");
		return ;
	}
	if (pid2 == 0)
	{
		dup2(fd[0], STDIN_FILENO);
		close(fd[0]);
		close(fd[1]);
		execve(find_command(cmds[1][0]), cmds[1], envp);
		perror("minishell: execve");
		exit(1);
	}
	close(fd[0]);
	close(fd[1]);
	waitpid(pid1, NULL, 0);
	waitpid(pid2, NULL, 0);
}

char	*find_command(char *command)
{
	char		*path_env;
	char		*paths;
	char		*token;
	char		full_path[PATH_MAX];
	struct stat	sb;

	path_env = getenv("PATH");
	if (!path_env)
		return (NULL);
	paths = ft_strdup(path_env);
	token = strtok(paths, ":");
	while (token)
	{
		snprintf(full_path, sizeof(full_path), "%s/%s", token, command);
		if (stat(full_path, &sb) == 0 && sb.st_mode & S_IXUSR)
		{
			free(paths);
			return (ft_strdup(full_path));
		}
		token = strtok(NULL, ":");
	}
	free(paths);
	return (NULL);
}

void	execute_command(char **args, char **envp)
{
	pid_t	pid;
	int		status;
	char	*path;

	pid = fork();
	if (pid == -1)
		return (perror("minishell: fork"));
	else if (pid == 0)
	{
		path = find_command(args[0]);
		if (!path)
		{
			fprintf(stderr, "minishell: command not found: %s\n", args[0]);
			exit(127);
		}
		if (execve(path, args, envp) == -1)
		{
			perror("minishell: execve");
			exit(1);
		}
	}
	else
		waitpid(pid, &status, 0);
}

#include "minishell.h"

void    ft_retranslate(t_command *cmd, char **envp)
{
    if (ft_strcmp(cmd->name, "echo") == 0)
        ft_echo(cmd->args);
    else if (ft_strcmp(cmd->name, "cd") == 0)
        ft_cd(cmd->args);
    else if (ft_strcmp(cmd->name, "pwd") == 0)
        ft_pwd();
    else if (ft_strcmp(cmd->name, "export") == 0)
        ft_export(cmd->args, &envp);
    else if (ft_strcmp(cmd->name, "unset") == 0)
        ft_unset(cmd->args, &envp);
    else if (ft_strcmp(cmd->name, "env") == 0)
        ft_env(envp);
    else if (ft_strcmp(cmd->name, "exit") == 0)
        ft_exit(cmd->args);
    else
        execute_command(cmd->args, envp);
    ft_free_args(cmd->args);
}


/*
Предлагаю типизировать все билд ины
а конкретно
функция куда попадает дерево
ft_tree_analys(tree **tree)
{
	далее смотрим сколько там "Групп". другими словами сколько пайпов
	можно какие то доп переменные
	и просто передаем указатель на группу в билд ин
	например
	< test1.txt ls -l | grep test | sort > output.txt
	здесь мы смотрим, есть 3 группы
	и у нас получается здесь три указателя
	< test1.txt ls -l | grep test | sort > output.txt
	|					|			|
	первый				второй 		третий
	то есть
	tree *ptr1, *ptr2, ptr3
	и таким образом
	ft_ls(tree *ptr1)
	ft_gret(tree *ptr2)
	ft_sort(tree *ptr3)
	выходит что мы их типизировали и не нужно думать че и куда мы будем передавать
	это также поможет дальше, когда мы будем вызывать комманды через execve
	мы передадим туда этот же поинтер
	no_builtin_func(tree *ptr)
	потому что execve берет:  -имя комманды, -аргументы, -переменные окружения
	и это изи все может быть в струтуре
}

*/

//  < test1.txt ls -l | grep test | sort > output.txt