/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/12 13:09:33 by vmamoten          #+#    #+#             */
/*   Updated: 2025/01/06 15:24:45 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

static char	*generate_heredoc_filename(void)
{
	static int	counter = 0;
	char		*filename;
	char		*number;
	int			fd;

	filename = NULL;
	while (1)
	{
		number = ft_itoa(counter);
		if (number == NULL)
			return (free(filename), NULL);
		free(filename);
		filename = ft_strjoin("/tmp/minishell_heredoc_", number);
		free(number);
		if (filename == NULL)
			return (NULL);
		fd = open(filename, O_CREAT | O_EXCL | O_WRONLY, 0600);
		if (fd >= 0)
			break ;
		counter++;
	}
	close(fd);
	counter++;
	return (filename);
}

int	read_heredoc_to_file(const char *delimiter, const char *tmpfile)
{
	int		out_fd;
	char	*line;

	out_fd = open(tmpfile, O_WRONLY | O_TRUNC, 0644);
	if (out_fd < 0)
	{
		perror("open temp heredoc file");
		return (0);
	}
	while (1)
	{
		line = readline("> ");
		if (!line)
			break ;
		if (ft_strcmp(line, delimiter) == 0)
		{
			free(line);
			break ;
		}
		write(out_fd, line, ft_strlen(line));
		write(out_fd, "\n", 1);
		free(line);
	}
	close(out_fd);
	return (1);
}

int	handle_heredoc(t_redirection *redir)
{
	char	*tmpfile;

	tmpfile = generate_heredoc_filename();
	if (!tmpfile)
		return (0);
	if (!read_heredoc_to_file(redir->filename, tmpfile))
	{
		free(tmpfile);
		return (0);
	}
	free(redir->filename);
	redir->filename = tmpfile;
	redir->type = TOKEN_REDIRECT_IN;
	return (1);
}

int	prepare_heredocs(t_exec_command *commands)
{
	t_exec_command	*cmd;
	t_redirection	*redir;

	cmd = commands;
	while (cmd)
	{
		redir = cmd->redirects;
		while (redir)
		{
			if (redir->type == TOKEN_HEREDOC)
			{
				if (!handle_heredoc(redir))
					return (0);
			}
			redir = redir->next;
		}
		cmd = cmd->next_cmd;
	}
	return (1);
}

static t_redirection	*reverse_redirections(t_redirection *head)
{
	t_redirection	*prev;
	t_redirection	*curr;
	t_redirection	*next;

	prev = NULL;
	curr = head;
	next = NULL;
	while (curr)
	{
		next = curr->next;
		curr->next = prev;
		prev = curr;
		curr = next;
	}
	return (prev);
}

int	open_redirection_file(t_redirection *redir)
{
	int	fd;

	if (redir->type == TOKEN_REDIRECT_OUT) // '>'
		fd = open(redir->filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	else if (redir->type == TOKEN_REDIRECT_APPEND) // '>>'
		fd = open(redir->filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
	else if (redir->type == TOKEN_REDIRECT_IN) // '<'
		fd = open(redir->filename, O_RDONLY);
	else
	{
		fprintf(stderr, "minishell: Unsupported redirection type\n");
		return (-1);
	}
	if (fd == -1)
		perror(redir->filename);
	return (fd);
}

int	apply_redirection(int fd, int target_fd)
{
	if (dup2(fd, target_fd) == -1)
	{
		perror("dup2");
		close(fd);
		return (0);
	}
	close(fd);
	return (1);
}

int	handle_redirections(t_redirection *redirects)
{
	t_redirection	*rev;
	int				fd;

	rev = reverse_redirections(redirects);
	while (rev)
	{
		fd = open_redirection_file(rev);
		if (fd == -1)
			return (0);
		if (rev->type == TOKEN_REDIRECT_IN)
		{
			if (!apply_redirection(fd, STDIN_FILENO))
				return (0);
		}
		else
		{
			if (!apply_redirection(fd, STDOUT_FILENO))
				return (0);
		}
		rev = rev->next;
	}
	return (1);
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
	ft_free_array(paths);
	if (!result)
		print_command_not_found(command);
	return (result);
}
