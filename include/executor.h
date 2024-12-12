/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/12 12:35:18 by vmamoten          #+#    #+#             */
/*   Updated: 2024/12/12 13:44:48 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXECUTOR_H
# define EXECUTOR_H

# include "minishell.h"

void	execute_commands(t_exec_command *commands, t_info *info);
void	execute_pipeline(t_exec_command *commands, t_info *info);
void	execute_single_command(t_exec_command *command, t_info *info);
void	execute_builtin(t_exec_command *command, t_info *info);
int		handle_redirections(t_redirection *redirects);
void	restore_standard_fds(int fd_in, int fd_out);
char	*find_command(char *command, char **envp);
void	execute_external_command(char **args, char **envp);

#endif
