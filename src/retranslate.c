/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   retranslate.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: admin <admin@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/22 11:19:30 by vmamoten          #+#    #+#             */
/*   Updated: 2024/10/13 15:23:17 by admin            ###   ########.fr       */
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
			while (j - i)
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

void	execute_pipeline(t_command *cmds, char **envp)
{
	int			num_cmds;
	int			i;
	int			j;
	pid_t		*pid;
	int			**fd;
	t_command	*current_cmd;

	num_cmds = 0;
	current_cmd = cmds;
	while (current_cmd)
	{
		num_cmds++;
		current_cmd = current_cmd->next;
	}
	pid = malloc(sizeof(pid_t) * num_cmds);
	if (!pid)
	{
		perror("malloc");
		return ;
	}
	fd = malloc(sizeof(int *) * (num_cmds - 1));
	if (!fd)
	{
		perror("malloc");
		free(pid);
		return ;
	}
	i = 0;
	while (i < num_cmds - 1)
	{
		fd[i] = malloc(sizeof(int *) * 2);
		if (pipe(fd[i]) == -1)
		{
			perror("pipe");
			while (i > 0)
			{
				close(fd[i - 1][0]);
				close(fd[i - 1][1]);
				free(fd[i - 1]);
				i--;
			}
			free(fd);
			free(pid);
			return ;
		}
		i++;
	}
	current_cmd = cmds;
	i = 0;
	while (current_cmd)
	{
		pid[i] = fork();
		if (pid[i] == -1)
		{
			perror("fork");
			return;
		}
		else if (pid[i] == 0)
		{
			if (i > 0)
			{
				if (dup2(fd[i - 1][0], STDIN_FILENO) == -1)
				{
					perror("dup2");
					exit(1);
				}
			}
		
			if (i < num_cmds - 1)
			{
				if (dup2(fd[i][1], STDOUT_FILENO) == -1)
				{
					perror("dup2");
					exit(1);
				}
			}
			j = 0;
			while (j < num_cmds - 1)
			{
				close(fd[j][0]);
				close(fd[j][1]);
				j++;
			}
			if (ft_strcmp(current_cmd->name, "echo") == 0)
				ft_echo(current_cmd->args);
			else if (ft_strcmp(current_cmd->name, "cd") == 0)
				ft_cd(current_cmd->args);
			else if (ft_strcmp(current_cmd->name, "pwd") == 0)
				ft_pwd();
			else if (ft_strcmp(current_cmd->name, "export") == 0)
				ft_export(current_cmd->args, &envp);
			else if (ft_strcmp(current_cmd->name, "unset") == 0)
				ft_unset(current_cmd->args, &envp);
			else if (ft_strcmp(current_cmd->name, "env") == 0)
				ft_env(envp);
			else if (ft_strcmp(current_cmd->name, "exit") == 0)
				ft_exit(current_cmd->args);
			else
			{
				char *path = find_command(current_cmd->args[0], envp);
				if (!path)
				{
					fprintf(stderr, "minishell: command not found: %s\n", current_cmd->args[0]);
					exit(127);
				}
				if (execve(path, current_cmd->args, envp) == -1)
				{
					perror("minishell: execve");
					exit(1);
				}
			}
			exit(0);
		}
		current_cmd = current_cmd->next;
		i++;
	}
	i = 0;
	while (i < num_cmds - 1)
	{
		close(fd[i][0]);
		close(fd[i][1]);
		free(fd[i]);
		i++;
	}
	if (num_cmds > 1)
		free(fd);
	i = 0;
	while (i < num_cmds)
	{
		waitpid(pid[i], NULL, 0);
		i++;
	}
	free(pid);
}
/*Подсчёт команд: Сначала мы считаем количество команд в списке t_command.
Выделение памяти: Выделяем память для массивов pid и fd.
Создание пайпов: Создаём необходимое количество пайпов.
Выполнение команд: Для каждой команды создаём дочерний процесс, настраиваем ввод/вывод через пайпы и выполняем команду.
Обработка встроенных команд: В дочерних процессах выполняем встроенные команды так же, как и внешние.
Закрытие пайпов: Закрываем все пайпы в родительском процессе.
Ожидание процессов: Ожидаем завершения всех дочерних процессов.*/


char	*find_command(char *command, char **envp)
{
	char		*path_env;
	char		**paths;
	char		*partial_path;
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
			break ;
		free(full_path);
		full_path = NULL;
		i++;
	}
	ft_free_args(paths);
	if (full_path)
		return (full_path);
	else
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
		path = find_command(args[0], envp);
		if (!path)
		{
			fprintf(stderr, "minishell: command not found: %s\n", args[0]);
			exit(127);
		}
		if (execve(path, args, envp) == -1)
		{
			perror("minishell: execve");
			free(path);
			exit(1);
		}
	}
	else
		waitpid(pid, &status, 0);
}

void	ft_retranslate(t_command *cmd, t_info *info, char **envp)
{
	if (info->pipes > 0)
	{
		execute_pipeline(cmd, envp);
	}
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