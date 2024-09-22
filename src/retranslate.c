/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   retranslate.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/22 11:19:30 by vmamoten          #+#    #+#             */
/*   Updated: 2024/09/22 13:11:52 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	ft_retranslate(t_tree *tree, char **argv)
{
	char	**args;

	args = argv;
	if (strcmp(tree->name, "echo") == 0)
		ft_echo(args);
	else if (strcmp(tree->name, "cd") == 0)
		ft_cd(args);
	else if (strcmp(tree->name, "pwd") == 0)
		ft_pwd();
}
//  < test1.txt ls -l | grep test | sort > output.txt