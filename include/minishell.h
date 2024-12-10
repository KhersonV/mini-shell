/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lynchsama <lynchsama@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/12 17:14:50 by vmamoten          #+#    #+#             */
/*   Updated: 2024/11/21 21:26:47 by lynchsama        ###   ########.fr       */
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
# include <sys/stat.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <unistd.h>

# define TRUE 1
# define FALSE 0

int					g_shell_interactive;

typedef struct s_info
{
	char			*input;
	int				exit_status;
	char			**envp;
	int				interactive;
	char			*old_dir;
	char			*curr_dir;
	int				std_in_reserve;
	int				std_out_reserve;
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

typedef struct s_redirection {
	char	*filename;
	int		fd;
	int		is_append;
	int		is_heredoc;
	char	*heredoc_marker;
	int		type;
	struct s_redirection *next;
} t_redirection;

typedef struct s_exec_command {
	char					*cmd_name;
	char					*exec_path;
	char					**args;
	int						pipe_fds[2];
	t_redirection			*redirects;
	int						exit_status;
	struct s_exec_command	*next_cmd;
	struct s_exec_command	*prev_cmd;
} t_exec_command;

enum				token_types
{
	TOKEN_SPACE = 1,
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
void				ft_echo(char **args, t_info *info);
void				ft_cd(char **args, char ***envp, t_info *info);
void				ft_pwd(t_info *info);
void				ft_export(char **args, char ***envp, t_info *info);
void				ft_unset(char **args, char ***envp, t_info *info);
void				ft_env(char **envp, t_info *info);
void				ft_exit(char **args, t_info *info);
void				execute_command(char **args, char **envp);
char				*find_command(char *command, char **envp);
void				ft_free_args(char **args);
void				ft_free_array(char **array);
void				free_ast(Node *node);
char				**copy_envp(char **envp);
void				execute_ast(Node *node, t_info *info);
void				execute_command_node(Node *node, t_info *info);
char				*get_env_value(char **envp, char *var);
void				signal_handler(int signo);
#endif
