/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   retranslate.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: admin <admin@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/22 11:19:30 by vmamoten          #+#    #+#             */
/*   Updated: 2024/09/22 23:26:11 by admin            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

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
	paths = strdup(path_env);
	token = strtok(paths, ":");
	while (token)
	{
		snprintf(full_path, sizeof(full_path), "%s/%s", token, command);
		if (stat(full_path, &sb) == 0 && sb.st_mode & S_IXUSR)
		{
			free(paths);
			return (strdup(full_path));
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

void	ft_retranslate(t_tree *tree, char **argv, char **envp)
{
	char	**args;

	args = argv;
	if (strcmp(tree->name, "echo") == 0)
		ft_echo(args);
	else if (strcmp(tree->name, "cd") == 0)
		ft_cd(args);
	else if (strcmp(tree->name, "pwd") == 0)
		ft_pwd();
	else
		execute_command(args, envp);
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