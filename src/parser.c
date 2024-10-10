/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lynchsama <lynchsama@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/07 12:53:34 by vmamoten          #+#    #+#             */
/*   Updated: 2024/10/10 21:58:14 by lynchsama        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "l_p.h"

typedef struct Node {
    char *data;
    struct Node *left;
    struct Node *right;
    char *args;
    char *redirect_op;
    char *redirect_file;
} Node;


int	ft_strcmp(const char *s1, const char *s2)
{
	while (*s1 && (*s1 == *s2))
	{
		s1++;
		s2++;
	}
	return ((unsigned char)*s1 - (unsigned char)*s2);
}



int is_command_or_argument(t_tree *token)
{
		return (!ft_strcmp(token->type, "WORD")
	|| !ft_strcmp(token->type, "FIELD")
	|| !ft_strcmp(token->type, "FILE")
	|| !ft_strcmp(token->type, "HEREDOC_MARKER")
	|| !ft_strcmp(token->type, "COMMAND")
	|| !ft_strcmp(token->type, "ARGUMENT")
	);
}

int is_operator(t_tree *token)
{
	return (!ft_strcmp(token->type, "REDIR_IN")
	|| !ft_strcmp(token->type, "REDIR_OUT")
	|| !ft_strcmp(token->type, "REDIR_APPEND")
	|| !ft_strcmp(token->type, "REDIR_INSOURCE")
	|| !ft_strcmp(token->type, "PIPE")
	);
}

int is_redir_operator(t_tree *token)
{
	return (!ft_strcmp(token->type, "REDIR_IN")
	|| !ft_strcmp(token->type, "REDIR_OUT")
	|| !ft_strcmp(token->type, "REDIR_APPEND")
	|| !ft_strcmp(token->type, "REDIR_INSOURCE")
	);
}

int is_file(t_tree *token)
{
	return (!ft_strcmp(token->type, "FILE")
	|| !ft_strcmp(token->type, "HEREDOC_MARKER")
	);
}

void print_ast(Node *root, int level) {
    if (root == NULL) {
        return;
    }

    for (int i = 0; i < level; i++) {
        printf("  ");
    }


    if (root->data != NULL) {
        printf("Node: %s\n", root->data);
    }


    if (root->args != NULL) {
        for (int i = 0; i < level + 1; i++) {
            printf("  ");
        }
        printf("Arguments: %s\n", root->args);
    }

    if (root->redirect_op != NULL && root->redirect_file != NULL) {
        for (int i = 0; i < level + 1; i++) {
            printf("  ");
        }
        printf("Redirect: %s -> %s\n", root->redirect_op, root->redirect_file);
    }


    if (root->left != NULL) {
        print_ast(root->left, level + 1);
    }
    if (root->right != NULL) {
        print_ast(root->right, level + 1);
    }
}

int skip_spaces(t_tree *token)
{
	return !ft_strcmp(token->type, "SPACE");
}
Node *create_node(char *data) {
    Node *new_node = (Node *)malloc(sizeof(Node));
    if (new_node == NULL) {
        printf("Memory allocation error\n");
        exit(1);
    }
    new_node->data = strdup(data);
    new_node->left = NULL;
    new_node->right = NULL;
    return new_node;
}

void attach_left(Node *root, Node *left_child) {
    if (root != NULL) {
        root->left = left_child;
    }
}

void attach_right(Node *root, Node *right_child) {
    if (root != NULL) {
        root->right = right_child;
    }
}

void attach_argument(Node *command, char *argument) {

    if (command != NULL && command->args == NULL) {
        command->args = strdup(argument);
    } else if (command != NULL) {
        char *new_args = malloc(strlen(command->args) + strlen(argument) + 2);
        strcpy(new_args, command->args);
        strcat(new_args, " ");
        strcat(new_args, argument);
        free(command->args);
        command->args = new_args;
    }
}

void attach_redirect(Node *command, char *redirect_op, char *file) {

    if (command != NULL) {
        command->redirect_op = strdup(redirect_op);
        command->redirect_file = strdup(file);
    }
}


Node *parse_tokens(t_tree *tokens) {
    Node *root = NULL;
    Node *left_command = NULL;
    Node *right_command = NULL;

    t_tree *curr = tokens;

    while (curr != NULL) {
        if (!(ft_strcmp(curr->type, "COMMAND"))) {
            if (root == NULL) {
                left_command = create_node(curr->name);  // Создаём левую команду
                root = left_command;                    // Если нет пайпа, команда становится корнем
            } else {
                right_command = create_node(curr->name); // Создаём правую команду
            }
        } else if (!(ft_strcmp(curr->type, "PIPE"))) {
            // PIPE становится корнем для левой и правой команды
            root = create_node("PIPE");
            attach_left(root, left_command);  // Левое поддерево
            left_command = NULL;              // Сбрасываем левую команду
        } else if (!(ft_strcmp(curr->type, "ARGUMENT"))) {
            // Аргумент команды
            attach_argument(left_command != NULL ? left_command : right_command, curr->name);
        } else if (is_redir_operator(curr)) {
            // Обрабатываем редирект и файл
            attach_redirect(left_command != NULL ? left_command : right_command, curr->name, curr->next->name);
            curr = curr->next;  // Пропускаем файл
        }
        curr = curr->next;
    }

    if (root != NULL && right_command != NULL) {
        attach_right(root, right_command);  // Связываем правую команду с корнем (для пайпа)
    }

    return root;
}
void remove_spaces(t_tree **tree) {
    t_tree *curr = *tree;

    while (curr != NULL) {

        if (ft_strcmp(curr->type, "SPACE") == 0) {
            t_tree *node_to_remove = curr;

            if (node_to_remove == *tree) {
                *tree = node_to_remove->next;
                if (*tree != NULL) {
                    (*tree)->prev = NULL;
                }
            } else {
                if (node_to_remove->prev != NULL) {
                    node_to_remove->prev->next = node_to_remove->next;
                }
                if (node_to_remove->next != NULL) {
                    node_to_remove->next->prev = node_to_remove->prev;
                }
            }

            curr = curr->next;
            free(node_to_remove);
        } else {
            curr = curr->next;
        }
    }
}

void adjusting_token_tree(t_tree **tree)
{
    t_tree *curr = *tree;
    int command_found = 0;

    while (curr != NULL)
    {
        if (!(ft_strcmp(curr->type, "PIPE"))) {
            command_found = 0;
        }

        if (!command_found && !(ft_strcmp(curr->type, "WORD"))) {
            curr->type = "COMMAND";
            command_found = 1;
        } else if (command_found && (!(ft_strcmp(curr->type, "WORD")) || command_found && !(ft_strcmp(curr->type, "FIELD")))) {
            curr->type = "ARGUMENT";
        }

        if (!(ft_strcmp(curr->type, "REDIR_IN"))
            || !(ft_strcmp(curr->type, "REDIR_OUT"))
            || !(ft_strcmp(curr->type, "REDIR_APPEND"))) {
            if (curr->next != NULL) {
                curr->next->type = "FILE";
            }
        } else if (!(ft_strcmp(curr->type, "REDIR_INSOURCE"))) {
            if (curr->next != NULL) {
                curr->next->type = "HEREDOC_MARKER";
            }
        }

        curr = curr->next;
    }
}



int main()
{
	char input[] = "echo \'hello\'	>> file.txt | cat << input.txt";
	//char input[] = "echo 'test' > output.txt | cat";
	//char input[] = "ls -l > output.txt";

	t_tree *root;
	Node *output;
	root = tokenize(input);
	remove_spaces(&root);
	adjusting_token_tree(&root);
	output = parse_tokens(root);
	//printf("after");
	print_tokens(root);
	printf("------\n");
	print_ast(output, 1);



	//shunting_yard(root);
	// Queue *output_queue = createQueue();
	// Stack *operator_stack = createStack();

	// print_queue(output_queue);
	// print_stack(operator_stack);


	return 0;
}