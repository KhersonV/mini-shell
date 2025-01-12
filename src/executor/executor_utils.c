/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/12 13:09:33 by vmamoten          #+#    #+#             */
/*   Updated: 2025/01/12 13:20:08 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

t_redirection	*reverse_redirections(t_redirection *head)
{
	t_redirection	*prev;
	t_redirection	*curr;
	t_redirection	*next;

	prev = NULL;
	curr = head;
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

	if (redir->type == TOKEN_REDIRECT_OUT)
		fd = open(redir->filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	else if (redir->type == TOKEN_REDIRECT_APPEND)
		fd = open(redir->filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
	else if (redir->type == TOKEN_REDIRECT_IN)
		fd = open(redir->filename, O_RDONLY);
	else
	{
		ft_putstr_fd("minishell: ", STDERR_FILENO);
		ft_putendl_fd("Unsupported redirection type", STDERR_FILENO);
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
		close(fd);
		return (0);
	}
	close(fd);
	return (1);
}

int	handle_redirections(t_redirection *redirects)
{
	t_redirection	*redir;
	int				fd;

	redir = redirects;
	while (redir)
	{
		fd = open_redirection_file(redir);
		if (fd == -1)
			return (0);
		if (redir->type == TOKEN_REDIRECT_IN)
		{
			if (!apply_redirection(fd, STDIN_FILENO))
				return (0);
		}
		else
		{
			if (!apply_redirection(fd, STDOUT_FILENO))
				return (0);
		}
		redir = redir->next;
	}
	return (1);
}

void	restore_standard_fds(int fd_in, int fd_out)
{
	if (fd_in != -1)
	{
		if (dup2(fd_in, STDIN_FILENO) == -1)
			close(fd_in);
	}
	if (fd_out != -1)
	{
		if (dup2(fd_out, STDOUT_FILENO) == -1)
			close(fd_out);
	}
}
