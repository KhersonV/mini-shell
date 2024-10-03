/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lynchsama <lynchsama@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/07 12:53:34 by vmamoten          #+#    #+#             */
/*   Updated: 2024/10/03 19:48:06 by lynchsama        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "l_p.h"

int	ft_strcmp(const char *s1, const char *s2)
{
	while (*s1 && (*s1 == *s2))
	{
		s1++;
		s2++;
	}
	return ((unsigned char)*s1 - (unsigned char)*s2);
}

Queue *createQueue()
{
	Queue *queue = (Queue *)malloc(sizeof(Queue));
	queue->front = queue->rear = NULL;
	return queue;
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

void shunting_yard(t_tree *tokens)
{

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

void push(Stack *stack, void *data)
{
	Node *node = createNode(data);
	node->next = stack->top;
	stack->top = node;
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

int isEmpty(Stack *stack)
{
	return stack->top == NULL;
}

void adjusting_token_tree(t_tree **tree)
{
	t_tree *curr = *tree;

	while(curr != NULL)
	{
		if(!(ft_strcmp(curr->type, "REDIR_IN"))
		|| !(ft_strcmp(curr->type, "REDIR_OUT"))
		|| !(ft_strcmp(curr->type, "REDIR_APPEND")))
		{
			if(!(ft_strcmp(curr->next->type, "SPACE")))
				curr = curr->next;
			if(curr->next != NULL )
			{
				curr->next->type = "FILE";
			}
		} else if(!(ft_strcmp(curr->type, "REDIR_INSOURCE")))
		{
			if(!(ft_strcmp(curr->next->type, "SPACE")))
				curr = curr->next;
			if(curr->next != NULL)
			{
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
	printf("before");
	print_tokens(root);
	adjusting_token_tree(&root);
	printf("after");
	print_tokens(root);
	return 0;
}