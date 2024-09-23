/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: admin <admin@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/12 17:14:50 by vmamoten          #+#    #+#             */
/*   Updated: 2024/09/23 20:02:53 by admin            ###   ########.fr       */
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
# include <string.h>
# include <sys/stat.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <unistd.h>

# define TRUE 1
# define FALSE 0

typedef struct s_command
{
	char *name;          // Имя команды
	char **args;         // Аргументы команды
	char *infile;        // Файл для перенаправления ввода
	char *outfile;       // Файл для перенаправления вывода
	int append;          // Флаг для '>>'
	struct s_command *next; // Указатель на следующую команду (для пайпов)
}		t_command;

void	ft_echo(char **args);
void	ft_cd(char **args);
void	ft_pwd(void);
void	ft_export(char **args, char ***env);
void	ft_unset(char **args);
void	ft_env(char **env);
void	ft_exit(char **args);
void	ft_retranslate(t_command *tree, char **args, char **envp);

#endif
