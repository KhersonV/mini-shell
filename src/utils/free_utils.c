/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/06 19:00:23 by vmamoten          #+#    #+#             */
/*   Updated: 2025/01/15 15:28:26 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

void	free_pointer(void **ptr)
{
	if (*ptr)
	{
		free(*ptr);
		*ptr = NULL;
	}
}

void	free_array(t_exec_command **commands)
{
	t_exec_command	*current;
	t_exec_command	*next;

	if (!commands || !*commands)
		return ;
	current = *commands;
	while (current)
	{
		next = current->next_cmd;
		free_pointer((void **)&current->cmd_name);
		free_pointer((void **)&current->exec_path);
		if (current->args)
			ft_free_array(&current->args);
		if (current->redirects)
			free_redirections(&(current->redirects));
		free(current);
		current = next;
	}
	*commands = NULL;
}

void	free_env(t_info *info)
{
	if (!info)
		return ;
	if (info->envp)
	{
		ft_free_array(&info->envp);
		info->envp = NULL;
	}
	if (info->old_dir)
	{
		free(info->old_dir);
		info->old_dir = NULL;
	}
	if (info->home)
	{
		free(info->home);
		info->home = NULL;
	}
}

void	free_info(t_info *info)
{
	if (!info)
		return ;
	free_pointer((void **)&info->input);
	free_pointer((void **)&info->old_dir);
	free_pointer((void **)&info->curr_dir);
	free_pointer((void **)&info->home);
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
		return ;
	j = 0;
	while (j < count)
	{
		if (env_copy[j])
		{
			free(env_copy[j]);
			env_copy[j] = NULL;
		}
		j++;
	}
	free(env_copy);
	env_copy = NULL;
}
