
#include "../minishell.h"

int launch_setup(int argc, t_info *info)
{
	if(argc != 1 && argc != 3)
		return -1;
	if(argc == 3)
	{
		info->interactive = 0;

	}
	else
		info->interactive = 1;
	return 1;
}

int main(int argc, char **argv, char **envp)
{
	t_info *info;

	info = malloc(sizeof(t_info));
	info->envp = copy_envp(envp);
	info->old_dir = get_env_value(envp, "OLDPWD");
	printf("%s", info->old_dir);
	info->exit_status = 0;

	return 0;
}