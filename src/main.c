/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/10 15:48:24 by vmamoten          #+#    #+#             */
/*   Updated: 2024/12/10 16:04:40 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"


int	main(int ac, char **av, char **envp)
{
	char		*line;
	t_exec_command	*commands;
	t_info		info;

	if (ac != 1)
		return (printf("minishell: %s: No such file or directory\n", av[1]), 1);
	init_env(&info, envp);
	init_signals();
	while (1)
	{
		line = readline("minishell> ");
		if (line == NULL) 
			exit_shell(&info); 
		if (*line != '\0')
			add_history(line);
		commands = parse(line);
		execute_commands(commands, &info);
		free_commands(commands);
		free(line);
	}
	return (0);
}
