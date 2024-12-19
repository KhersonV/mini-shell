/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/07 12:53:34 by vmamoten          #+#    #+#             */
/*   Updated: 2024/12/19 12:38:50 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"


// Проверка на пробельный токен
int skip_spaces(t_token *token)
{
    return (token->type == TOKEN_SPACE);
}

#include "../../include/minishell.h"

t_exec_command *ast_to_exec_commands(t_node *ast)
{
    t_exec_command *command;
    t_exec_command *head;

    if (!ast)
        return (NULL);
    head = NULL;
    while (ast)
    {
        command = malloc(sizeof(t_exec_command));
        if (!command)
        {
            perror("malloc");
            return (NULL);
        }
        command->cmd_name = ft_strdup(ast->data);
        command->args = ft_split(ast->args, ' '); // Разбивает аргументы на массив
        command->redirects = NULL; // Обработка редиректов добавляется при необходимости
        command->next_cmd = NULL;
        if (head)
        {
            t_exec_command *temp = head;
            while (temp->next_cmd)
                temp = temp->next_cmd;
            temp->next_cmd = command;
        }
        else
            head = command;
        ast = ast->right; // Переход к следующему узлу AST
    }
    return (head);
}


// Удаление пробелов из списка токенов
void remove_spaces(t_token **tree)
{
    t_token *curr;
    t_token *node_to_remove;

    curr = *tree;
    while (curr != NULL)
    {
        if (curr->type == TOKEN_SPACE)
        {
            node_to_remove = curr;
            if (node_to_remove == *tree)
            {
                *tree = node_to_remove->next;
                if (*tree != NULL)
                    (*tree)->prev = NULL;
            }
            else
            {
                if (node_to_remove->prev != NULL)
                    node_to_remove->prev->next = node_to_remove->next;
                if (node_to_remove->next != NULL)
                    node_to_remove->next->prev = node_to_remove->prev;
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

// Настройка дерева токенов
void adjusting_token_tree(t_token **tree)
{
    t_token *curr;
    int command_found;

    curr = *tree;
    command_found = 0;
    while (curr != NULL)
    {
        if (curr->type == TOKEN_PIPE)
        {
            command_found = 0;
        }
        if (!command_found && curr->type == TOKEN_WORD)
        {
            curr->type = TOKEN_COMMAND;
            command_found = 1;
        }
        else if (command_found && (curr->type == TOKEN_WORD || curr->type == TOKEN_FIELD ||
                                   curr->type == TOKEN_EXP_FIELD || curr->type == TOKEN_VAR))
        {
            curr->type = TOKEN_ARGUMENT;
        }
        if (curr->type == TOKEN_REDIRECT_IN || curr->type == TOKEN_REDIRECT_OUT ||
            curr->type == TOKEN_REDIRECT_APPEND)
        {
            if (curr->next != NULL)
                curr->next->type = TOKEN_FILE;
        }
        else if (curr->type == TOKEN_HEREDOC)
        {
            if (curr->next != NULL)
                curr->next->type = TOKEN_HEREDOC_MARKER;
        }
        curr = curr->next;
    }
}

// Создание узла AST
t_node *create_node(char *data)
{
    t_node *new_node = (t_node *)malloc(sizeof(t_node));
    if (!new_node)
    {
        perror("Memory allocation failed");
        exit(1);
    }
    new_node->data = strdup(data);
    new_node->left = NULL;
    new_node->right = NULL;
    new_node->args = NULL;
    new_node->redirect_op = NULL;
    new_node->redirect_file = NULL;
    return new_node;
}

// Добавление аргументов к команде
void attach_argument(t_node *command, char *argument)
{
    char *new_args;

    if (command && !command->args)
    {
        command->args = strdup(argument);
    }
    else if (command)
    {
        new_args = malloc(strlen(command->args) + strlen(argument) + 2);
        if (!new_args)
        {
            perror("Memory allocation failed");
            exit(1);
        }
        strcpy(new_args, command->args);
        strcat(new_args, " ");
        strcat(new_args, argument);
        free(command->args);
        command->args = new_args;
    }
}

// Добавление редиректов к команде
void attach_redirect(t_node *command, char *redirect_op, char *file)
{
    if (command)
    {
        command->redirect_op = strdup(redirect_op);
        command->redirect_file = strdup(file);
    }
}

// Построение AST из токенов
t_node *parse_tokens(t_token *tokens)
{
    t_node *root = NULL;
    t_node *current_command = NULL;
    t_token *curr = tokens;

    while (curr)
    {
        if (curr->type == TOKEN_COMMAND)
        {
            if (!current_command)
                current_command = create_node(curr->str);
        }
        else if (curr->type == TOKEN_ARGUMENT)
        {
            attach_argument(current_command, curr->str);
        }
        else if (curr->type == TOKEN_PIPE)
        {
            t_node *pipe_node = create_node("|");
            if (root)
                pipe_node->left = root;
            else
                pipe_node->left = current_command;
            root = pipe_node;
            current_command = NULL;
        }
        else if (curr->type == TOKEN_REDIRECT_IN || curr->type == TOKEN_REDIRECT_OUT ||
                 curr->type == TOKEN_REDIRECT_APPEND || curr->type == TOKEN_HEREDOC)
        {
            attach_redirect(current_command, curr->str, curr->next->str);
            curr = curr->next;
        }
        curr = curr->next;
    }
    if (root)
        return root;
    return current_command;
}

// Вывод AST для отладки
void print_ast(t_node *root, int level)
{
    if (!root)
        return;
    int i = 0;
    while (i < level)
    {
        printf("  ");
        i++;
    }
    printf("Node: %s\n", root->data);
    if (root->args)
    {
        i = 0;
        while (i < level + 1)
        {
            printf("  ");
            i++;
        }
        printf("Arguments: %s\n", root->args);
    }
    if (root->redirect_op && root->redirect_file)
    {
        i = 0;
        while (i < level + 1)
        {
            printf("  ");
            i++;
        }
        printf("Redirect: %s -> %s\n", root->redirect_op, root->redirect_file);
    }
    print_ast(root->left, level + 1);
    print_ast(root->right, level + 1);
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