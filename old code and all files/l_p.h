#ifndef L_P_H
#define L_P_H

#include <stdio.h>

typedef struct s_tree
{
	char 			*type;
	char			*name;
	int				precedence;
	struct s_tree	*next;
	struct s_tree	*prev;
}					t_tree;

typedef struct s_token
{
	char	*str;
	int		type;
	int		precedence;
	struct s_token *next;
	struct s_token *prev;
}	t_token;

typedef struct Node
{
	char			*data;
	struct Node		*left;
	struct Node		*right;
	char			*args;
	char			*redirect_op;
	char			*redirect_file;
}					Node;




enum token_types {
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
typedef struct s_redirection {
	char	*filename;
	int		fd;
	int		is_append;
	int		is_heredoc;
	char	*heredoc_marker;
}	t_redirection;

typedef struct s_argument_list {
	char					*arg_value;
	struct s_argument_list	*next_arg;
}	t_argument_list;

typedef struct s_exec_command {
	char				*cmd_name;
	char				*exec_path;
	t_argument_list		*args_list;
	int				has_pipe;
	int					pipe_fds[2];
	t_redirection		*input_redirect;
	t_redirection		*output_redirect;
	int					exit_status;
	struct s_exec_command	*next_cmd;
	struct s_exec_command	*prev_cmd;
}	t_exec_command;



void print_tokens(t_tree *node);
t_tree *tokenize(char *s);
char	*ft_strdup(const char *s1);
Node	*parse_tokens(t_tree *tokens);
void	print_ast(Node *root, int level);
void	adjusting_token_tree(t_tree **tree);
void	remove_spaces(t_tree **tree);




#endif