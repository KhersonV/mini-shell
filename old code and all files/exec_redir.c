/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_redir.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/23 19:30:30 by vmamoten          #+#    #+#             */
/*   Updated: 2024/09/25 14:15:57 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	execute_command_with_redirect(char **args, char *outfile, int append,
		char **envp)
{
	pid_t	pid;
	int		status;
	int		fd;

	pid = fork();
	if (pid == -1)
		return (perror("minishell: fork"));
	else if (pid == 0)
	{
		if (append)
			fd = open(outfile, O_WRONLY | O_CREAT | O_APPEND, 0644);
		else
			fd = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if (fd == -1)
		{
			perror("minishell: open");
			exit(1);
		}
		dup2(fd, STDOUT_FILENO);
		close(fd);
		execve(find_command(args[0], envp), args, envp);
		perror("minishell: execve");
		exit(1);
	}
	else
		waitpid(pid, &status, 0);
}
