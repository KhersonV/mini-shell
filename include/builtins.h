/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/10 14:26:53 by vmamoten          #+#    #+#             */
/*   Updated: 2024/12/10 14:37:59 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#if !defined(BUILTINS_H)
# define BUILTINS_H
# include "minishell.h"
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>

void	ft_echo(char **args, t_info *info);
void	ft_cd(char **args, char ***envp, t_info *info);
void	ft_pwd(t_info *info);
void	ft_export(char **args, char ***envp, t_info *info);
void	ft_unset(char **args, char ***envp, t_info *info);
void	ft_env(char **envp, t_info *info);
void	ft_exit(char **args, t_info *info);

char	**copy_envp(char **envp);
char	*get_env_value(char **envp, char *var);
void	ft_free_args(char **args);
void	ft_free_array(char **array);

#endif
