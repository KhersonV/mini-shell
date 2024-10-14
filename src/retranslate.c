/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   retranslate.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: admin <admin@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/22 11:19:30 by vmamoten          #+#    #+#             */
/*   Updated: 2024/10/14 14:47:18 by admin            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

// void	execute_command_with_redirect(char **args, char *outfile, int append,
// 		char **envp)
// {
// 	pid_t	pid;
// 	int		status;
// 	int		fd;

// 	pid = fork();
// 	if (pid == -1)
// 		return (perror("minishell: fork"));
// 	else if (pid == 0)
// 	{
// 		if (append)
// 			fd = open(outfile, O_WRONLY | O_CREAT | O_APPEND, 0644);
// 		else
// 			fd = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
// 		if (fd == -1)
// 		{
// 			perror("minishell: open");
// 			exit(1);
// 		}
// 		dup2(fd, STDOUT_FILENO);
// 		close(fd);
// 		execve(find_command(args[0]), args, envp);
// 		perror("minishell: execve");
// 		exit(1);
// 	}
// 	else
// 		waitpid(pid, &status, 0);
// }

char	**copy_envp(char **envp)
{
	int		i;
	int		j;
	char	**env_copy;

	i = 0;
	while (envp[i])
		i++;
	env_copy = malloc(sizeof(char *) * (i + 1));
	if (!env_copy)
		return (NULL);
	i = 0;
	while (envp[i])
	{
		env_copy[i] = ft_strdup(envp[i]);
		if (!env_copy[i])
		{
			j = 0;
			while (j < i)
			{
				free(env_copy[j]);
				j++;
			}
			free(env_copy);
			return (NULL);
		}
		i++;
	}
	env_copy[i] = NULL;
	return (env_copy);
}

void	execute_pipe(Node *node, t_info *info)
{
	int		fd[2];
	pid_t	pid1;
	pid_t	pid2;

	if (pipe(fd) == -1)
	{
		perror("pipe");
		return ;
	}
	pid1 = fork();
	if (pid1 == -1)
	{
		perror("fork");
		return ;
	}
	if (pid1 == 0)
	{
		close(fd[0]);
		if (dup2(fd[1], STDOUT_FILENO) == -1)
		{
			perror("dup2");
			exit(1);
		}
		close(fd[1]);
		execute_ast(node->left, info);
		exit(0);
	}
	pid2 = fork();
	if (pid2 == -1)
	{
		perror("fork");
		return ;
	}
	if (pid2 == 0)
	{
		close(fd[1]);
		if (dup2(fd[0], STDIN_FILENO) == -1)
		{
			perror("dup2");
			exit(1);
		}
		close(fd[0]);
		execute_ast(node->right, info);
		exit(0);
	}
	close(fd[0]);
	close(fd[1]);
	waitpid(pid1, NULL, 0);
	waitpid(pid2, NULL, 0);
}

char	*find_command(char *command, char **envp)
{
	char		*path_env;
	char		**paths;
	char		*full_path;
	struct stat	sb;
	int			i;

	(void)envp;
	path_env = getenv("PATH");
	if (!path_env)
		return (NULL);
	paths = ft_split(path_env, ':');
	if (!paths)
		return (NULL);
	i = 0;
	full_path = NULL;
	while (paths[i])
	{
		full_path = ft_strjoin(ft_strjoin(paths[i], "/"), command);
		if (!full_path)
		{
			i++;
			continue ;
		}
		if (stat(full_path, &sb) == 0 && sb.st_mode & S_IXUSR)
		{
			ft_free_array(paths);
			return (full_path);
		}
		free(full_path);
		full_path = NULL;
		i++;
	}
	ft_free_array(paths);
	return (NULL);
}

void	execute_command(char **args, char **envp)
{
	char	*path;

		path = find_command(args[0], envp);
		if (!path)
		{
			ft_putstr_fd("minishell: command not found: ", 2);
			ft_putstr_fd(args[0], 2);
			ft_putstr_fd("\n", 2);
			exit(127);
		}
		if (execve(path, args, envp) == -1)
		{
			perror("minishell: execve");
			free(path);
			exit(1);
		}
	free(path);
}

void	execute_command_node(Node *node, t_info *info)
{
	(void)node;
	(void)info;
}

void	execute_ast(Node *node, t_info *info)
{
	if (!node)
		return ;
	if (ft_strcmp(node->data, "PIPE") == 0)
		execute_pipe(node, info);
	else
		execute_command_node(node, info);
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