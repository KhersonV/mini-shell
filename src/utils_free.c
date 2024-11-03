#include "../minishell.h"

void free_info_struct(t_info *info)
{
	free(info->curr_dir);
	free(info->old_dir);
	free(info->envp);
	free(info);
}