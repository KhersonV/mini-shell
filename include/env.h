/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/11 12:44:01 by vmamoten          #+#    #+#             */
/*   Updated: 2024/12/11 12:56:16 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ENV_H
# define ENV_H

# include "minishell.h"
# include <stdlib.h>

void	init_env(t_info *info, char **envp);

char	*get_env_value(t_info *info, const char *key);
int		env_key_compare(const char *env_entry, const char *key);
char	*get_value_from_env(const char *env_entry);
void	set_env(t_info *info, const char *key, const char *value);
char	*create_env_entry(const char *key, const char *value);
void	unset_env(t_info *info, const char *key);
char	**append_env_entry(char **env, const char *entry);
char	**env_to_array(t_info *info);

void	free_env(t_info *info);

#endif
