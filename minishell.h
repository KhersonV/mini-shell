/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/12 17:14:50 by vmamoten          #+#    #+#             */
/*   Updated: 2024/09/12 17:16:53 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include <errno.h>
# include <readline/history.h>
# include <readline/readline.h>
# include <signal.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <unistd.h>

# define TRUE 1
# define FALSE 0

void ft_echo(char **args);
void ft_cd(char **args);
void ft_pwd(void);
void ft_export(char **args, char ***env);
void ft_unset(char **args, char ***env);
void ft_env(char **env);
void ft_exit(char **args);

#endif
