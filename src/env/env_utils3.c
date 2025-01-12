/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_utils3.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/06 18:34:33 by vmamoten          #+#    #+#             */
/*   Updated: 2025/01/12 18:08:59 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

void	init_env(t_info *info)
{
	int		shlvl;
	char	new_shlvl[12];

	if (!info->envp)
		exit(EXIT_FAILURE);
	remove_oldpwd(&(info->envp));
	shlvl = calculate_shlvl(get_env_value(info, "SHLVL"));
	shlvl_to_string(shlvl, new_shlvl);
	if (set_env(info, "SHLVL", new_shlvl) == -1)
		exit(EXIT_FAILURE);
}

char	*get_env_value(t_info *info, const char *key)
{
	int		i;
	char	*equals;

	i = 0;
	while (info->envp[i])
	{
		if (env_key_compare(info->envp[i], key))
		{
			equals = ft_strchr(info->envp[i], '=');
			if (!equals)
				return (NULL);
			return (ft_strdup(equals + 1));
		}
		i++;
	}
	return (NULL);
}

int	is_valid_env_key(const char *key)
{
	int	i;

	i = 0;
	if (!key || !key[0] || ft_isdigit(key[0]))
		return (0);
	while (key[i])
	{
		if (!(ft_isalnum(key[i]) || key[i] == '_'))
			return (0);
		i++;
	}
	return (1);
}

char	**env_to_array(t_info *info)
{
	return (copy_envp(info->envp));
}
