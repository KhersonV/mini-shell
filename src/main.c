/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: admin <admin@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/07 11:46:57 by vmamoten          #+#    #+#             */
/*   Updated: 2024/10/14 15:40:47 by admin            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	ft_free_args(char **args)
{
	int	i;

	i = 0;
	if (!args)
		return ;
	while (args[i])
	{
		free(args[i]);
		i++;
	}
	free(args);
}

void	ft_free_array(char **array)
{
	int	i;

	i = 0;
	if (!array)
		return ;
	while (array[i])
	{
		free(array[i]);
		i++;
	}
	free(array);
}

void	free_token_list(t_tree *tokens)
{
	t_tree	*temp;

	while (tokens)
	{
		temp = tokens;
		tokens = tokens->next;
		free(temp->name);
		free(temp->type);
		free(temp);
	}
}

void	free_ast(Node *node)
{
	if (!node)
		return ;
	if (node->data)
		free(node->data);
	if (node->args)
		free(node->args);
	if (node->redirect_op)
		free(node->redirect_op);
	if (node->redirect_file)
		free(node->redirect_file);
	if (node->left)
	{
		free_ast(node->left);
	}
	if (node->right)
	{
		free_ast(node->right);
	}
	free(node);
}

int	main(int argc, char **argv, char **envp)
{
	char	*line;
	t_tree	*tokens;
	Node	*ast_root;
	t_info	info;

	(void)argc;
	(void)argv;
	info.envp = copy_envp(envp);
	if (!info.envp)
	{
		perror("Failed to copy environment");
		return (1);
	}
	while (1)
	{
		line = readline("minishell> ");
		if (!line)
			break ;
		if (*line)
			add_history(line);
		tokens = tokenize(line);
		free(line);
		if (!tokens)
			continue ;
		remove_spaces(&tokens);
		adjusting_token_tree(&tokens);
		ast_root = parse_tokens(tokens);
		free_token_list(tokens);
		if (!ast_root)
			continue ;
		execute_ast(ast_root, &info);
		free_ast(ast_root);
	}
	ft_free_args(info.envp);
	return (0);
}
