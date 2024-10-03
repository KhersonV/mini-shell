#ifndef L_P_H
#define L_P_H

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
	void *data;
	struct Node *next;
} Node;

typedef struct Stack
{
	Node *top;
} Stack;

typedef struct Queue
{
	Node *front;
	Node *rear;
} Queue;

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

void print_tokens(t_tree *node);
t_tree *tokenize(char *s);

#endif