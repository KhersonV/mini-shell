/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/12 17:14:50 by vmamoten          #+#    #+#             */
/*   Updated: 2024/12/12 13:52:16 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include "../libft/libft.h"
# include "env.h"
# include "executor.h"
# include "lexer.h"
# include "parser.h"
# include "signals.h"
# include <errno.h>
# include <fcntl.h>
# include <limits.h>
# include <sys/stat.h>
# include <sys/types.h>
# include <sys/wait.h>

# define TRUE 1
# define FALSE 0

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
	int						pipe_fds[2];
	t_redirection			*redirects;
	int						exit_status;
	struct s_exec_command	*next_cmd;
	struct s_exec_command	*prev_cmd;
}							t_exec_command;

#endif
