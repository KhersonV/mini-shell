/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: admin <admin@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/12 17:14:50 by vmamoten          #+#    #+#             */
/*   Updated: 2024/10/12 15:25:36 by admin            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include "libft/libft.h"
# include <errno.h>
# include <fcntl.h>
# include <limits.h>
# include <readline/history.h>
# include <readline/readline.h>
# include <signal.h>
# include <stdio.h>
# include <stdlib.h>
# include <sys/stat.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <unistd.h>

# define TRUE 1
# define FALSE 0

typedef struct s_info
{
	int					pipes;
	char				**envp;

}						t_info;

typedef struct s_command
{
	char				*name;
	char				**args;
	char				*file;
	char *redirection; // >> || > ||<< ||<
	int					pipes;
	struct s_command	*next;
}						t_command;

void					ft_echo(char **args);
void					ft_cd(char **args);
void					ft_pwd(void);
void					ft_export(char **args, char ***env);
void					ft_unset(char **args, char ***envp);
void					ft_env(char **env);
void					ft_exit(char **args);
void					ft_retranslate(t_command *cmd,t_info *info, char **envp);
void					execute_command_with_redirect(char **args,
							char *outfile, int append, char **envp);
void					execute_command(char **args, char **envp);
char					*find_command(char *command, char **envp);
void					ft_free_args(char **args);
char					**copy_envp(char **envp);

#endif
