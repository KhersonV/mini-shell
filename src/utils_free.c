#include "../minishell.h"

void free_info_struct(t_info *info)
{
	free(info->curr_dir);
	free(info->old_dir);
	free(info->envp);
	free(info);
}

void free_two_dim_array(char **arr)
{
	int	i;

	i = 0;
	while(arr[i])
	{
		free(arr[i]);
		i++;
	}
	free(arr);
}