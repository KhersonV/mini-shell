/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   herodoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/07 14:11:30 by vmamoten          #+#    #+#             */
/*   Updated: 2025/01/12 13:19:11 by vmamoten         ###   ########.fr       */
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

int	write_heredoc_line(int out_fd, const char *delimiter)
{
	char	*line;

	while (1)
	{
		line = readline("> ");
		if (!line)
			return (0);
		if (ft_strcmp(line, delimiter) == 0)
		{
			free(line);
			break ;
		}
		write(out_fd, line, ft_strlen(line));
		write(out_fd, "\n", 1);
		free(line);
	}
	return (1);
}

int	read_heredoc_to_file(const char *delimiter, const char *tmpfile)
{
	int	out_fd;

	out_fd = open(tmpfile, O_WRONLY | O_TRUNC | O_CREAT, 0600);
	if (out_fd < 0)
	{
		perror("open temp heredoc file");
		return (0);
	}
	set_signal_mode_heredoc();
	if (!write_heredoc_line(out_fd, delimiter))
	{
		close(out_fd);
		return (0);
	}
	set_signal_mode_readline();
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
		cmd->redirects = reverse_redirections(cmd->redirects);
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
