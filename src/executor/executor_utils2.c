/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_utils2.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/07 14:09:15 by vmamoten          #+#    #+#             */
/*   Updated: 2025/01/15 15:16:03 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

int	is_builtin(char *command)
{
	if (!command)
		return (0);
	return (ft_strcmp(command, "echo") == 0 || ft_strcmp(command, "cd") == 0
		|| ft_strcmp(command, "pwd") == 0 || ft_strcmp(command, "export") == 0
		|| ft_strcmp(command, "unset") == 0 || ft_strcmp(command, "env") == 0
		|| ft_strcmp(command, "ENV") == 0 || ft_strcmp(command, "exit") == 0);
}

char	*check_absolute_path(char *command)
{
	struct stat	statbuf;

	if (stat(command, &statbuf) == 0)
	{
		if (S_ISDIR(statbuf.st_mode))
		{
			ft_putendl_fd("minishell: /: is a directory", STDERR_FILENO);
			return (NULL);
		}
		if (access(command, X_OK) == 0)
			return (ft_strdup(command));
	}
	ft_putendl_fd("minishell: /: No such file or directory", STDERR_FILENO);
	return (NULL);
}

char	*search_in_path(char *command, char **paths)
{
	struct stat	statbuf;
	char		*full_path;
	char		*temp;
	int			i;

	i = 0;
	while (paths[i])
	{
		temp = ft_strjoin(paths[i], "/");
		full_path = ft_strjoin(temp, command);
		free(temp);
		if (stat(full_path, &statbuf) == 0)
		{
			if (S_ISDIR(statbuf.st_mode))
			{
				ft_putendl_fd("minishell: /: is a directory", STDERR_FILENO);
				return (free(full_path), NULL);
			}
			if (access(full_path, X_OK) == 0)
				return (full_path);
		}
		free(full_path);
		i++;
	}
	return (NULL);
}

void	print_command_not_found(char *command)
{
	char	*prefix;
	char	*message;

	prefix = ft_strjoin("minishell: ", command);
	if (!prefix)
		return ;
	message = ft_strjoin(prefix, ": command not found\n");
	free(prefix);
	if (!message)
		return ;
	ft_putstr_fd(message, STDERR_FILENO);
	free(message);
}

char	*find_command(char *command, char **envp)
{
	char	*path_env;
	char	**paths;
	char	*result;

	result = NULL;
	if (ft_strchr(command, '/'))
		return (check_absolute_path(command));
	path_env = get_env_value_direct(envp, "PATH");
	if (!path_env || path_env[0] == '\0')
	{
		print_command_not_found(command);
		return (NULL);
	}
	paths = ft_split(path_env, ':');
	free(path_env);
	if (!paths)
		return (NULL);
	result = search_in_path(command, paths);
	ft_free_array(&paths);
	if (!result)
		print_command_not_found(command);
	return (result);
}
