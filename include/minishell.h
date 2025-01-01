/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lynchsama <lynchsama@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/12 17:14:50 by vmamoten          #+#    #+#             */
/*   Updated: 2025/01/01 23:12:27 by lynchsama        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include "../libft/libft.h"
# include "../get_next_line/get_next_line.h"
# include <fcntl.h>
# include <limits.h>
# include <stdio.h>
# include <readline/history.h>
# include <readline/readline.h>
# include <signal.h>
# include <stdlib.h>
# include <string.h>
# include <sys/stat.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <unistd.h>

/* Макросы */
# define TRUE 1
# define FALSE 0

/* Структуры */
typedef struct s_info
{
	char					*input;
	int						exit_status;
	char					**envp;
	char					*old_dir;
	char					*curr_dir;
	int						std_in_reserve;
	int						std_out_reserve;
}							t_info;

typedef struct s_redirection
{
	char					*filename;
	int						fd;
	int						is_append;
	int						is_heredoc;
	char					*heredoc_marker;
	int						type;
	struct s_redirection	*next;
}							t_redirection;

typedef struct s_exec_command
{
	char					*cmd_name;
	char					*exec_path;
	char					**args;
	t_redirection			*redirects;
	int						exit_status;
	struct s_exec_command	*next_cmd;
	struct s_exec_command	*prev_cmd;
}							t_exec_command;

typedef struct s_token
{
	char					*str;
	int						type;
	struct s_token			*next;
	struct s_token			*prev;
}							t_token;



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
	TOKEN_COMMAND,
	TOKEN_ARGUMENT,
	TOKEN_FIELD,
	TOKEN_EXP_FIELD,
	TOKEN_VAR,
	TOKEN_FILE,
	TOKEN_HEREDOC_MARKER,
	TOKEN_EXIT_STATUS
}							t_token_type;

/* Прототипы функций */

t_exec_command				*build_fake_command_list(void);
t_exec_command				*parse_tokens_to_commands(t_token *tokens);


void						print_command_list(t_exec_command *cmd_list);
/* Builtins */
void						ft_echo(t_exec_command *command, t_info *info);
void						ft_cd(char **args, t_info *info);
void						ft_pwd(t_info *info);
void						ft_export(char **args, t_info *info);
void						unset_env(char **args, t_info *info);
void						ft_env(t_exec_command *commands, t_info *info);
void						ft_exit(char **args, t_info *info);

/* Env */
char						**copy_envp(char **envp);
void						init_env(t_info *info, char **envp);
char						*get_env_value(t_info *info, const char *key);
int							set_env(t_info *info, const char *key, const char *value);
char						**env_to_array(t_info *info);


/* Env utils */

int							is_valid_env_key(const char *key);
int							env_key_compare(const char *env_entry,
								const char *key);
char						*get_value_from_env(const char *env_entry);
char 						*get_env_value_direct(char **envp, const char *key);

char						*create_env_entry(const char *key,
								const char *value);
char						**append_env_entry(char **env, const char *entry);
char						**remove_env_entry(char **env, int index);

void						exit_shell(t_info *info);

/* Executor */
void						execute_commands(t_exec_command *commands,
								t_info *info);
void						execute_pipeline(t_exec_command *commands,
								t_info *info);
void						execute_single_command(t_exec_command *command,
								t_info *info);
void						execute_builtin(t_exec_command *command,
								t_info *info);
int							handle_redirections(t_redirection *redirects);
void						restore_standard_fds(int fd_in, int fd_out);
char						*find_command(char *command, char **envp);
int							is_builtin(char *command);

void						execute_commands(t_exec_command *commands, t_info *info);

/* Lexer */
t_token						*tokenize(char *input);
void						adjusting_token_tree(t_token **tree);
void						free_token_list(t_token *tokens);
void						expansion(t_token **tokens, t_info *info);


/* Signals */
void						init_signals(void);
void						reset_signals_to_default(void);

/* utils - free_utils */
void						ft_free_array(char **array);

void						free_env_array(char **env);
void						free_commands(t_exec_command *commands);

void						free_env(t_info *info);
void						free_redirections(t_redirection *redirects);
#endif
