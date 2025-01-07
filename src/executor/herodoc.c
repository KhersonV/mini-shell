/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   herodoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/07 14:11:30 by vmamoten          #+#    #+#             */
/*   Updated: 2025/01/07 14:16:38 by vmamoten         ###   ########.fr       */
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

int	create_heredoc_file(const char *heredoc_marker)
{
	char	*filename;
	int		fd;
	char	*line;

	filename = "/tmp/minishell_heredoc";
	fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd == -1)
	{
		perror("open");
		return (-1);
	}
	while (1)
	{
		line = readline("> ");
		if (!line || strcmp(line, heredoc_marker) == 0)
		{
			free(line);
			break ;
		}
		write(fd, line, strlen(line));
		write(fd, "\n", 1);
		free(line);
	}
	close(fd);
	return (open(filename, O_RDONLY));
}
