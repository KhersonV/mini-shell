/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/12 13:09:33 by vmamoten          #+#    #+#             */
/*   Updated: 2024/12/12 13:45:55 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/executor.h"
#include "../../include/lexer.h"

int	handle_redirections(t_redirection *redirects)
{
	int	fd;

	while (redirects)
	{
		if (redirects->type == TOKEN_REDIRECT_IN)
			fd = open(redirects->filename, O_RDONLY);
		else if (redirects->type == TOKEN_REDIRECT_OUT)
			fd = open(redirects->filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		else if (redirects->type == TOKEN_REDIRECT_APPEND)
			fd = open(redirects->filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
		else
		{
			fprintf(stderr, "Unsupported redirection type\n");
			return (0);
		}
		if (fd == -1)
		{
			perror(redirects->filename);
			return (0);
		}
		if (dup2(fd,
				(redirects->type == TOKEN_REDIRECT_IN) ? STDIN_FILENO : STDOUT_FILENO) ==
			-1)
		{
			perror("dup2");
			close(fd);
			return (0);
		}
		close(fd);
		redirects = redirects->next;
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
	return (strcmp(command, "echo") == 0 || strcmp(command, "cd") == 0
		|| strcmp(command, "pwd") == 0 || strcmp(command, "export") == 0
		|| strcmp(command, "unset") == 0 || strcmp(command, "env") == 0
		|| strcmp(command, "exit") == 0);
}
