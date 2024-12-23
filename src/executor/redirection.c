/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/12 13:12:26 by vmamoten          #+#    #+#             */
/*   Updated: 2024/12/23 13:51:55 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

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
