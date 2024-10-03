/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lynchsama <lynchsama@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/07 12:53:34 by vmamoten          #+#    #+#             */
/*   Updated: 2024/10/03 17:08:50 by lynchsama        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node
{
	void *data;
	struct Node *next;
} Node;

typedef struct Stack
{
	Node *top;
} Stack;

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
		return ;
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
		return ;
	}
	return stack->top->data;
}

int isEmpty(Stack *stack)
{
	return stack->top == NULL;
}

int main()
{

	return 0;
}