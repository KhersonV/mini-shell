/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/12 13:09:33 by vmamoten          #+#    #+#             */
/*   Updated: 2025/01/06 14:24:11 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

static char	*generate_heredoc_filename(void)
{
	char	*filename;
	int		fd;

	filename = ft_strdup("/tmp/minishell_heredoc_XXXXXX");
	fd = mkstemp(filename);
	if (fd == -1)
	{
		perror("mkstemp");
		free(filename);
		return (NULL);
	}
	close(fd);
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

int	prepare_heredocs(t_exec_command *commands)
{
	t_exec_command	*cmd;
	t_redirection	*redir;
	char			*tmpfile;

	cmd = commands;
	while (cmd)
	{
		redir = cmd->redirects;
		while (redir)
		{
			if (redir->type == TOKEN_HEREDOC)
			{
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

int	handle_redirections(t_redirection *redirects)
{
	int				fd;
	t_redirection	*rev;

	rev = reverse_redirections(redirects);
	while (rev)
	{
		if (rev->type == TOKEN_REDIRECT_OUT) // '>'
			fd = open(rev->filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		else if (rev->type == TOKEN_REDIRECT_APPEND) // '>>'
			fd = open(rev->filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
		else if (rev->type == TOKEN_REDIRECT_IN) // '<'
			fd = open(rev->filename, O_RDONLY);
		else
		{
			fprintf(stderr, "minishell: Unsupported redirection type\n");
			return (0);
		}
		if (fd == -1)
		{
			perror(rev->filename);
			return (0);
		}
		if (rev->type == TOKEN_REDIRECT_IN)
		{
			if (dup2(fd, STDIN_FILENO) == -1)
			{
				perror("dup2");
				close(fd);
				return (0);
			}
		}
		else
		{
			if (dup2(fd, STDOUT_FILENO) == -1)
			{
				perror("dup2");
				close(fd);
				return (0);
			}
		}
		close(fd);
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

char	*find_command(char *command, char **envp)
{
	struct stat	statbuf;
	char		*path_env;
	char		**paths;
	char		*full_path;
	char		*temp;
	int			i;

	if (ft_strchr(command, '/'))
	{
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
	path_env = get_env_value_direct(envp, "PATH");
	if (!path_env || path_env[0] == '\0')
	{
		fprintf(stderr, "minishell: %s: command not found\n", command);
		return (NULL);
	}
	paths = ft_split(path_env, ':');
	free(path_env);
	if (!paths)
		return (NULL);
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
				ft_free_array(paths);
				free(full_path);
				return (NULL);
			}
			if (access(full_path, X_OK) == 0)
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
	return (NULL);
}
