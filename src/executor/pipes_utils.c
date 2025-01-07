/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipes_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/07 13:59:10 by vmamoten          #+#    #+#             */
/*   Updated: 2025/01/07 14:00:00 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

int	count_commands(t_exec_command *commands)
{
	int	count;

	count = 0;
	while (commands)
	{
		count++;
		commands = commands->next_cmd;
	}
	return (count);
}

int	**init_pipes(int num_cmds)
{
	int	**pipes;
	int	i;

	pipes = ft_calloc(num_cmds - 1, sizeof(int *));
	if (!pipes)
		return (NULL);
	i = 0;
	while (i < num_cmds - 1)
	{
		pipes[i] = ft_calloc(2, sizeof(int));
		if (!pipes[i] || pipe(pipes[i]) == -1)
			return (NULL);
		i++;
	}
	return (pipes);
}

void	free_pipes(int **pipes, int num_cmds)
{
	int	i;

	i = 0;
	while (i < num_cmds - 1)
	{
		if (pipes[i])
		{
			close(pipes[i][0]);
			close(pipes[i][1]);
			free(pipes[i]);
		}
		i++;
	}
	free(pipes);
}
