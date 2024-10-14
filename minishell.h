/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: admin <admin@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/12 17:14:50 by vmamoten          #+#    #+#             */
/*   Updated: 2024/10/14 14:51:35 by admin            ###   ########.fr       */
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
	int				pipes;
	char			**envp;

}					t_info;

typedef struct Node
{
	char			*data;
	struct Node		*left;
	struct Node		*right;
	char			*args;
	char			*redirect_op;
	char			*redirect_file;
}					Node;

typedef struct s_tree
{
	char			*type;
	char			*name;
	int				precedence;
	struct s_tree	*next;
	struct s_tree	*prev;
}					t_tree;

typedef struct s_token
{
	char			*str;
	int				type;
	int				precedence;
	struct s_token	*next;
	struct s_token	*prev;
}					t_token;

enum				token_types
{
	SPACE = 1,
	WORD,
	FIELD,
	EXP_FIELD,
	REDIR_OUT,
	REDIR_IN,
	REDIR_APPEND,
	REDIR_INSOURCE,
	PIPE,
	END
};

void				print_tokens(t_tree *node);

int					skip_spaces(t_tree *token);
Node				*parse_tokens(t_tree *tokens);
void				remove_spaces(t_tree **tree);
t_tree				*tokenize(char *s);
void				adjusting_token_tree(t_tree **tree);
void				ft_echo(char **args);
void				ft_cd(char **args);
void				ft_pwd(void);
void				ft_export(char **args, char ***env);
void				ft_unset(char **args, char ***envp);
void				ft_env(char **env);
void				ft_exit(char **args);
void				execute_command(char **args, char **envp);
char				*find_command(char *command, char **envp);
void				ft_free_args(char **args);
void				ft_free_array(char **array);
char				**copy_envp(char **envp);
void				execute_ast(Node *node, t_info *info);
void				execute_command_node(Node *node, t_info *info);

#endif
