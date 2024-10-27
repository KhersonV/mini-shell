/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: admin <admin@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/07 11:46:57 by vmamoten          #+#    #+#             */
/*   Updated: 2024/10/27 17:13:45 by admin            ###   ########.fr       */
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

void	signal_handler(int signo)
{
	if (signo == SIGINT)
	{
		write(1, "\n", 1);
		rl_on_new_line();
		rl_replace_line("", 0);
		rl_redisplay();
	}
}

int	main(int argc, char **argv, char **envp)
{
	t_tree				*tokens;
	Node				*ast_root;
	t_info				info;
	struct sigaction	sa;

	sa.sa_handler = signal_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	sigaction(SIGINT, &sa, NULL);
	signal(SIGQUIT, SIG_IGN);
	(void)argc;
	(void)argv;
	info.envp = copy_envp(envp);
	info.exit_status = 0;
	if (!info.envp)
	{
		perror("Failed to copy environment");
		return (1);
	}
	while (1)
	{
		info.input = readline("minishell> ");
		if (!info.input)
		{
			write(1, "exit\n", 5);
			break ;
		}
		if (*info.input)
			add_history(info.input);
		tokens = tokenize(info.input);
		free(info.input);
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
