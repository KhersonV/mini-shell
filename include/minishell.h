/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/12 17:14:50 by vmamoten          #+#    #+#             */
/*   Updated: 2024/12/14 13:26:23 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <signal.h>
# include <fcntl.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <sys/stat.h>
# include <string.h>
# include <limits.h>
# include "../libft/libft.h"

/* Макросы */
# define TRUE 1
# define FALSE 0

/* Структуры */
typedef struct s_info
{
    char    *input;
    int     exit_status;
    char    **envp;
    char    *old_dir;
    char    *curr_dir;
    int     std_in_reserve;
    int     std_out_reserve;
}               t_info;

typedef struct s_redirection
{
    char                    *filename;
    int                     fd;
    int                     is_append;
    int                     is_heredoc;
    char                    *heredoc_marker;
    int                     type;
    struct s_redirection    *next;
}               t_redirection;

typedef struct s_exec_command
{
    char                    *cmd_name;
    char                    *exec_path;
    char                    **args;
    int                     pipe_fds[2]; 
    t_redirection           *redirects;
    int                     exit_status;
    struct s_exec_command   *next_cmd;
    struct s_exec_command   *prev_cmd;
}               t_exec_command;

typedef struct s_token
{
    char            *str;
    int             type;
    struct s_token  *next;
    struct s_token  *prev;
}               t_token;

typedef struct s_node
{
    char            *data;
    struct s_node   *left;
    struct s_node   *right;
    char            *args;
    char            *redirect_op;
    char            *redirect_file;
}               t_node;

/* Перечисления */
typedef enum e_token_type
{
    TOKEN_WORD,
    TOKEN_PIPE,
    TOKEN_REDIRECT_IN,
    TOKEN_REDIRECT_OUT,
    TOKEN_REDIRECT_APPEND,
    TOKEN_HEREDOC,
    TOKEN_SPACE,
    TOKEN_END
}               t_token_type;

/* Прототипы функций */

/* Builtins */
void    ft_echo(char **args, t_info *info);
void    ft_cd(char **args, char ***envp, t_info *info);
void    ft_pwd(t_info *info);
void    ft_export(char **args, t_info *info);
void    ft_unset(char **args, t_info *info);
void    ft_env(char **envp, t_info *info);
void    ft_exit(char **args, t_info *info);

/* Env */
void    init_env(t_info *info, char **envp);
char    *get_env_value(t_info *info, const char *key);
void    set_env(t_info *info, const char *key, const char *value);
void    unset_env(t_info *info, const char *key);
char    **env_to_array(t_info *info);
void    free_env(t_info *info);

/* Env utils */
int     is_valid_env_key(const char *key);
int     env_key_compare(const char *env_entry, const char *key);
char    *get_value_from_env(const char *env_entry);
char    *create_env_entry(const char *key, const char *value);
char    **append_env_entry(char **env, const char *entry);
char    **remove_env_entry(char **env, int index);

/* Executor */
void    execute_commands(t_exec_command *commands, t_info *info);
void    execute_pipeline(t_exec_command *commands, t_info *info);
void    execute_single_command(t_exec_command *command, t_info *info);
void    execute_builtin(t_exec_command *command, t_info *info);
int     handle_redirections(t_redirection *redirects);
void    restore_standard_fds(int fd_in, int fd_out);
char    *find_command(char *command, char **envp);
void    execute_external_command(char **args, char **envp);

/* Lexer */
t_token *tokenize(char *input);
void    free_tokens(t_token *tokens);
void    print_tokens(t_token *tokens);

/* Parser */
t_node  *parse_tokens(t_token *tokens);
void    free_ast(t_node *node);
void    print_ast(t_node *node);

/* Signals */
void    init_signals(void);
void    reset_signals_to_default(void);

#endif
