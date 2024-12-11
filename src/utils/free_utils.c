
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