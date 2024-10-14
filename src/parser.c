/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: admin <admin@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/07 12:53:34 by vmamoten          #+#    #+#             */
/*   Updated: 2024/10/14 16:06:34 by admin            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int	ft_strcmp(const char *s1, const char *s2)
{
	while (*s1 && (*s1 == *s2))
	{
		s1++;
		s2++;
	}
	return ((unsigned char)*s1 - (unsigned char)*s2);
}

int	is_command_or_argument(t_tree *token)
{
	return (!ft_strcmp(token->type, "WORD") || !ft_strcmp(token->type, "FIELD")
		|| !ft_strcmp(token->type, "FILE") || !ft_strcmp(token->type,
			"HEREDOC_MARKER") || !ft_strcmp(token->type, "COMMAND")
		|| !ft_strcmp(token->type, "ARGUMENT"));
}

int	is_operator(t_tree *token)
{
	return (!ft_strcmp(token->type, "REDIR_IN") || !ft_strcmp(token->type,
			"REDIR_OUT") || !ft_strcmp(token->type, "REDIR_APPEND")
		|| !ft_strcmp(token->type, "REDIR_INSOURCE") || !ft_strcmp(token->type,
			"PIPE"));
}

int	is_redir_operator(t_tree *token)
{
	return (!ft_strcmp(token->type, "REDIR_IN") || !ft_strcmp(token->type,
			"REDIR_OUT") || !ft_strcmp(token->type, "REDIR_APPEND")
		|| !ft_strcmp(token->type, "REDIR_INSOURCE"));
}

int	is_file(t_tree *token)
{
	return (!ft_strcmp(token->type, "FILE") || !ft_strcmp(token->type,
			"HEREDOC_MARKER"));
}

void	print_ast(Node *root, int level)
{
	if (root == NULL)
	{
		return ;
	}
	for (int i = 0; i < level; i++)
	{
		printf("  ");
	}
	if (root->data != NULL)
	{
		printf("Node: %s\n", root->data);
	}
	if (root->args != NULL)
	{
		for (int i = 0; i < level + 1; i++)
		{
			printf("  ");
		}
		printf("Arguments: %s\n", root->args);
	}
	if (root->redirect_op != NULL && root->redirect_file != NULL)
	{
		for (int i = 0; i < level + 1; i++)
		{
			printf("  ");
		}
		printf("Redirect: %s -> %s\n", root->redirect_op, root->redirect_file);
	}
	if (root->left != NULL)
	{
		print_ast(root->left, level + 1);
	}
	if (root->right != NULL)
	{
		print_ast(root->right, level + 1);
	}
}
int	skip_spaces(t_tree *token)
{
	return (!ft_strcmp(token->type, "SPACE"));
}
Node	*create_node(char *data)
{
	Node	*new_node;

	new_node = (Node *)malloc(sizeof(Node));
	if (new_node == NULL)
	{
		printf("Memory allocation error\n");
		exit(1);
	}
	new_node->data = strdup(data);
	new_node->left = NULL;
	new_node->right = NULL;
	new_node->args = NULL;
	new_node->redirect_op = NULL;
	new_node->redirect_file = NULL;
	return (new_node);
}

void	attach_left(Node *root, Node *left_child)
{
	if (root != NULL)
	{
		root->left = left_child;
	}
}

void	attach_right(Node *root, Node *right_child)
{
	if (root != NULL)
	{
		root->right = right_child;
	}
}

void	attach_argument(Node *command, char *argument)
{
	char	*new_args;

	if (command != NULL && command->args == NULL)
	{
		command->args = strdup(argument);
	}
	else if (command != NULL)
	{
		new_args = malloc(strlen(command->args) + strlen(argument) + 2);
		strcpy(new_args, command->args);
		strcat(new_args, " ");
		strcat(new_args, argument);
		free(command->args);
		command->args = new_args;
	}
}

void	attach_redirect(Node *command, char *redirect_op, char *file)
{
	if (command != NULL)
	{
		command->redirect_op = strdup(redirect_op);
		command->redirect_file = strdup(file);
	}
}

Node	*parse_tokens(t_tree *tokens)
{
	Node	*root;
	Node	*current_command;
	Node	*last_pipe;
	t_tree	*curr;
	Node	*new_pipe;

	root = NULL;
	current_command = NULL;
	last_pipe = NULL;
	curr = tokens;
	while (curr != NULL)
	{
		if (!(ft_strcmp(curr->type, "COMMAND")))
		{
			if (current_command == NULL)
			{
				current_command = create_node(curr->name);
				if (root == NULL)
				{
					root = current_command;
				}
			}
		}
		else if (!(ft_strcmp(curr->type, "PIPE")))
		{
			new_pipe = create_node("PIPE");
			if (last_pipe == NULL)
			{
				attach_left(new_pipe, root);
			}
			else
			{
				attach_right(last_pipe, current_command);
				attach_left(new_pipe, last_pipe);
			}
			last_pipe = new_pipe;
			current_command = NULL;
		}
		else if (!(ft_strcmp(curr->type, "ARGUMENT")))
		{
			attach_argument(current_command, curr->name);
		}
		else if (is_redir_operator(curr))
		{
			attach_redirect(current_command, curr->name, curr->next->name);
			curr = curr->next;
		}
		else
		{
			free_ast(root);
			return (NULL);
		}
	curr = curr->next;
	}
	if (last_pipe != NULL && current_command != NULL)
	{
		attach_right(last_pipe, current_command);
	}
	if (last_pipe == NULL)
	{
		return (root);
	}
	return (last_pipe);
}

void	remove_spaces(t_tree **tree)
{
	t_tree	*curr;
	t_tree	*node_to_remove;

	curr = *tree;
	while (curr != NULL)
	{
		if (ft_strcmp(curr->type, "SPACE") == 0)
		{
			node_to_remove = curr;
			if (node_to_remove == *tree)
			{
				*tree = node_to_remove->next;
				if (*tree != NULL)
				{
					(*tree)->prev = NULL;
				}
			}
			else
			{
				if (node_to_remove->prev != NULL)
				{
					node_to_remove->prev->next = node_to_remove->next;
				}
				if (node_to_remove->next != NULL)
				{
					node_to_remove->next->prev = node_to_remove->prev;
				}
			}
			curr = curr->next;
			free(node_to_remove);
		}
		else
		{
			curr = curr->next;
		}
	}
}

void	adjusting_token_tree(t_tree **tree)
{
	t_tree	*curr;
	int		command_found;

	curr = *tree;
	command_found = 0;
	while (curr != NULL)
	{
		if (!(ft_strcmp(curr->type, "PIPE")))
		{
			command_found = 0;
		}
		if (!command_found && !(ft_strcmp(curr->type, "WORD")))
		{
			free(curr->type);
			curr->type = ft_strdup("COMMAND");
			command_found = 1;
		}
		else if (command_found && (!(ft_strcmp(curr->type, "WORD"))
				|| (command_found && !(ft_strcmp(curr->type, "FIELD")))))
		{
			free(curr->type);
			curr->type = ft_strdup("ARGUMENT");
		}
		if (!(ft_strcmp(curr->type, "REDIR_IN")) || !(ft_strcmp(curr->type,
					"REDIR_OUT")) || !(ft_strcmp(curr->type, "REDIR_APPEND")))
		{
			if (curr->next != NULL)
			{
				free(curr->next->type);
				curr->next->type = ft_strdup("FILE");
			}
		}
		else if (!(ft_strcmp(curr->type, "REDIR_INSOURCE")))
		{
			if (curr->next != NULL)
			{
				free(curr->next->type);
				curr->next->type = ft_strdup("HEREDOC_MARKER");
			}
		}
		curr = curr->next;
	}
}

// int	main(void)
// {
// 	// char input[] = "echo \'hello\'	>> file.txt | cat << input.txt | ls >> out.txt | grep 'hi' ";
// 	// char input[] = "echo 'test' > output.txt | cat";
// 	char input[] = "ls -l > output.txt";

// 	t_tree *root;
// 	Node *output;
// 	root = tokenize(input);
// 	remove_spaces(&root);
// 	adjusting_token_tree(&root);
// 	output = parse_tokens(root);
// 	// printf("after");
// 	print_tokens(root);
// 	printf("------\n");
// 	print_ast(output, 1);

// 	return (0);
// }