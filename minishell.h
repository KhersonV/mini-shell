/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: admin <admin@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/12 17:14:50 by vmamoten          #+#    #+#             */
/*   Updated: 2024/09/23 19:27:54 by admin            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include "libft/libft.h"
# include <stdio.h>
# include <errno.h>
# include <fcntl.h>
# include <limits.h>
# include <readline/history.h>
# include <readline/readline.h>
# include <signal.h>
# include <stdlib.h>
# include <string.h>
# include <sys/stat.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <unistd.h>

# define TRUE 1
# define FALSE 0

typedef struct s_tree
{
	char			*type;
	char			*name;
	struct s_tree	*left;
	struct s_tree	*right;
}					t_tree;

void				ft_echo(char **args);
void				ft_cd(char **args);
void				ft_pwd(void);
void				ft_export(char **args, char ***env);
void				ft_unset(char **args);
void				ft_env(char **env);
void				ft_exit(char **args);
void				ft_retranslate(t_tree *tree, char **args, char **envp);

#endif
