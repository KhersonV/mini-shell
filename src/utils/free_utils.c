/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/06 19:00:23 by vmamoten          #+#    #+#             */
/*   Updated: 2025/01/12 14:45:08 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"


void	free_redirections(t_redirection *redirects)
{
	t_redirection	*temp;

	while (redirects)
	{
		temp = redirects;
		free(redirects->filename);
		redirects = redirects->next;
		free(temp);
	}
}

void	free_array(t_exec_command *commands)
{
	t_exec_command	*current;
	t_exec_command	*next;

	current = commands;
	while (current)
	{
		next = current->next_cmd;
		free(current->cmd_name);
		free(current->exec_path);
		ft_free_array(current->args);
		free_redirections(current->redirects);
		free(current);
		current = next;
	}
}

void	free_env(t_info *info)
{
	int	i;

	if (!info || !info->envp)
		return ;
	i = 0;
	while (info->envp[i])
	{
		free(info->envp[i]);
		i++;
	}
	free(info->envp);
	info->envp = NULL;
}

void	free_info(t_info *info)
{
	if (!info)
		return ;
	free(info->input);
	free(info->old_dir);
	free(info->curr_dir);
	free(info->home);
	free_env(info);
	info->nb_cmds = 0;
	info->exit_status = 0;
	info->syntax_error = 0;
	info->std_in_reserve = -1;
	info->std_out_reserve = -1;
}

void	free_partial_env_copy(char **env_copy, int count)
{
	int	j;

	if (!env_copy)
		return;
	j = 0;
	while (j < count)
	{
		free(env_copy[j]);
		j++;
	}
	free(env_copy);
}
