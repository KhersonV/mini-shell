/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   retranslate.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: admin <admin@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/22 11:19:30 by vmamoten          #+#    #+#             */
/*   Updated: 2024/10/14 16:33:13 by admin            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

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
	char		*partial_path;
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
		partial_path = ft_strjoin(paths[i], "/");
		if (!partial_path)
		{
			i++;
			continue ;
		}
		full_path = ft_strjoin(partial_path, command);
		free(partial_path);
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
	pid_t	pid;
	int		status;
	char	**args;
	char	**args_split;
	int		arg_count;
	int		i;
	int		fd_in;
	int		fd_out;

	fd_in = -1;
	fd_out = -1;
	if (node->args)
	{
		args_split = ft_split(node->args, ' ');
		arg_count = 0;
		while (args_split[arg_count])
			arg_count++;
		args = malloc(sizeof(char *) * (arg_count + 2));
		if (!args)
		{
			perror("malloc");
			ft_free_args(args_split);
			return ;
		}
		args[0] = ft_strdup(node->data);
		i = 0;
		while (i < arg_count)
		{
			args[i + 1] = ft_strdup(args_split[i]);
			i++;
		}
		args[arg_count + 1] = NULL;
		ft_free_args(args_split);
	}
	else
	{
		args = malloc(sizeof(char *) * 2);
		if (!args)
		{
			perror("malloc");
			return ;
		}
		args[0] = ft_strdup(node->data);
		args[1] = NULL;
	}
	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		ft_free_args(args);
		return ;
	}
	if (pid == 0)
	{
		if (node->redirect_op)
		{
			if (ft_strcmp(node->redirect_op, "<") == 0)
				fd_in = open(node->redirect_file, O_RDONLY);
			else if (ft_strcmp(node->redirect_op, ">") == 0)
				fd_out = open(node->redirect_file, O_WRONLY | O_CREAT | O_TRUNC,
						0644);
			else if (ft_strcmp(node->redirect_op, ">>") == 0)
				fd_out = open(node->redirect_file,
						O_WRONLY | O_CREAT | O_APPEND, 0644);
			else
			{
				ft_putstr_fd("Unsupported redirection operator\n", 2);
				exit(1);
			}
			if ((fd_in == -1 && node->redirect_op[0] == '<') || (fd_out == -1
					&& node->redirect_op[0] == '>'))
			{
				perror("open");
				exit(1);
			}
			if (fd_in != -1)
			{
				if (dup2(fd_in, STDIN_FILENO) == -1)
				{
					perror("dup2");
					exit(1);
				}
				close(fd_in);
			}
			if (fd_out != -1)
			{
				if (dup2(fd_out, STDOUT_FILENO) == -1)
				{
					perror("dup2");
					exit(1);
				}
				close(fd_out);
			}
		}
		if (ft_strcmp(args[0], "echo") == 0)
			ft_echo(args);
		else if (ft_strcmp(args[0], "cd") == 0)
			ft_cd(args);
		else if (ft_strcmp(args[0], "pwd") == 0)
			ft_pwd();
		else if (ft_strcmp(args[0], "export") == 0)
			ft_export(args, &(info->envp));
		else if (ft_strcmp(args[0], "unset") == 0)
			ft_unset(args, &(info->envp));
		else if (ft_strcmp(args[0], "env") == 0)
			ft_env(info->envp);
		else if (ft_strcmp(args[0], "exit") == 0)
			ft_exit(args);
		else
			execute_command(args, info->envp);
		ft_free_args(args);
		exit(0);
	}
	else
	{
		waitpid(pid, &status, 0);
	}
	ft_free_args(args);
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