/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipes.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/12 13:10:51 by vmamoten          #+#    #+#             */
/*   Updated: 2025/01/06 18:27:55 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

// int	create_pipe(int fd[2])
// {
// 	if (pipe(fd) == -1)
// 	{
// 		perror("pipe");
// 		return (0);
// 	}
// 	return (1);
// }

// void	close_pipe(int fd[2])
// {
// 	close(fd[0]);
// 	close(fd[1]);
// }

// int	setup_pipe_io(int fd_in, int fd_out)
// {
// 	if (fd_in != -1)
// 	{
// 		if (dup2(fd_in, STDIN_FILENO) == -1)
// 		{
// 			perror("dup2");
// 			return (0);
// 		}
// 	}
// 	if (fd_out != -1)
// 	{
// 		if (dup2(fd_out, STDOUT_FILENO) == -1)
// 		{
// 			perror("dup2");
// 			return (0);
// 		}
// 	}
// 	return (1);
// }
