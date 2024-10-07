/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lynchsama <lynchsama@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/07 12:53:34 by vmamoten          #+#    #+#             */
/*   Updated: 2024/10/07 22:18:49 by lynchsama        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "l_p.h"

void print_queue(Queue *queue) {
    Node *current = queue->front;
    printf("Queue content:\n");
    while (current != NULL) {
        t_tree *token = (t_tree *)current->data;
        printf("Token: %s, Type: %s, Precedence: %d\n", token->name, token->type, token->precedence);
        current = current->next;
    }
}

void print_stack(Stack *stack) {
    Node *current = stack->top;
    printf("Stack content:\n");
    while (current != NULL) {
        t_tree *token = (t_tree *)current->data;
        printf("Token: %s, Type: %s, Precedence: %d\n", token->name, token->type, token->precedence);
        current = current->next;
    }
}

int	ft_strcmp(const char *s1, const char *s2)
{
	while (*s1 && (*s1 == *s2))
	{
		s1++;
		s2++;
	}
	return ((unsigned char)*s1 - (unsigned char)*s2);
}

int isEmpty(Stack *stack)
{
	return stack->top == NULL;
}

Queue *createQueue()
{
	Queue *queue = (Queue *)malloc(sizeof(Queue));
	queue->front = queue->rear = NULL;
	return queue;
}


void *dequeue(Queue *queue)
{
	if(queue->front == NULL)
	{
		printf("Queue is empty");
		return NULL;
	}

	Node *temp = queue->front;
	void *data = temp->data;

	if(queue->front == NULL)
	{
		queue->rear = NULL;
	}

	free(temp);
	return data;
}

int isEmptyQueue(Queue *queue)
{
	return queue->front == NULL;
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


Stack *createStack()
{
	Stack *stack = (Stack *)malloc(sizeof(Stack));
	stack->top = NULL;
	return stack;
}

Node *createNode(void *data)
{
	Node *node = (Node *)malloc(sizeof(Node));
	node->data = data;
	node->next  =NULL;
	return node;
}
void enqueue(Queue *queue, void *data)
{
	Node *node = createNode(data);
	if(queue->rear == NULL)
	{
		queue->front = queue->rear = node;
		return ;
	}
	queue->rear->next = node;
	queue->rear = node;
}

int precedence(void *data)
{
	t_tree *token = (t_tree *)data;
	return token->precedence;
}

void *pop(Stack *stack)
{
	if(stack->top == NULL)
	{
		printf("Stack is empty");
		return NULL;
	}

	Node *temp = stack->top;
	void *poppedData = temp->data;
	stack->top = stack->top->next;
	free(temp);
	return poppedData;
}

void *peek(Stack *stack)
{
	if(stack->top == NULL)
	{
		printf("stack is empty");
		return NULL;
	}
	return stack->top->data;
}

int skip_spaces(t_tree *token)
{
	return !ft_strcmp(token->type, "SPACE");
}


void push(Stack *stack, void *data)
{
	Node *node = createNode(data);
	node->next = stack->top;
	stack->top = node;
}

void shunting_yard(t_tree *tokens) {

    Stack *operator_stack = createStack();
    Queue *output_queue = createQueue();

    t_tree *curr = tokens;

    while (curr != NULL) {

        if (is_command_or_argument(curr)) {
            enqueue(output_queue, (void *)curr);
        } else if (is_operator(curr)) {
            while (!isEmpty(operator_stack) &&
                   precedence(peek(operator_stack)) >= curr->precedence) {
                enqueue(output_queue, pop(operator_stack));
            }
            push(operator_stack, (void *)curr);
        }
        curr = curr->next;
    }

    // Перемещаем все оставшиеся операторы из стека в очередь
    while (!isEmpty(operator_stack)) {
        enqueue(output_queue, pop(operator_stack));
    }




	 print_queue(output_queue);
	 print_stack(operator_stack);
	 printf("--------------------\n");
	 //print_tokens(tokens);
}

// void adjusting_token_tree(t_tree **tree)
// {
// 	t_tree *curr = *tree;

// 	while(curr != NULL)
// 	{
// 		if(!(ft_strcmp(curr->type, "REDIR_IN"))
// 		|| !(ft_strcmp(curr->type, "REDIR_OUT"))
// 		|| !(ft_strcmp(curr->type, "REDIR_APPEND")))
// 		{
// 			if(!(ft_strcmp(curr->next->type, "SPACE")))
// 				curr = curr->next;
// 			if(curr->next != NULL )
// 			{
// 				curr->next->type = "FILE";
// 			}
// 		} else if(!(ft_strcmp(curr->type, "REDIR_INSOURCE")))
// 		{
// 			if(!(ft_strcmp(curr->next->type, "SPACE")))
// 				curr = curr->next;
// 			if(curr->next != NULL)
// 			{
// 				curr->next->type = "HEREDOC_MARKER";
// 			}
// 		}

// 		curr = curr->next;
// 	}
// }

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
        } else if (command_found && !(ft_strcmp(curr->type, "WORD"))) {
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
	//char input[] = "echo hello >> \"file.txt\" | cat << input.txt";

	t_tree *root;
	root = tokenize(input);
	remove_spaces(&root);
	adjusting_token_tree(&root);
	//printf("after");
	//print_tokens(root);

	shunting_yard(root);
	// Queue *output_queue = createQueue();
	// Stack *operator_stack = createStack();

	// print_queue(output_queue);
	// print_stack(operator_stack);


	return 0;
}