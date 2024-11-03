
#include "../minishell.h"

/*
 TODO: add comparing envp's arrras function
 TODO: add free two dim array function (or find it)
*/



void free_info_struct(t_info *info);
int	ft_strcmp(const char *s1, const char *s2);


// TODO : init.c line:60
// int launch_setup(int argc, t_info *info)
// {
// 	if(argc != 1 && argc != 3)
// 		return 0;
// 	if(argc == 3)
// 	{
// 		info->interactive = 0;

// 	}
// 	else
// 		info->interactive = 1;
// 	return 1;
// }

int envp_compare(char **saved, char **current)
{
	int curr_len = 0;
	int saved_len = 0;
	int i = 0;

	while (current[curr_len])
		curr_len++;
	while (saved[saved_len])
		saved_len++;
	if (curr_len != saved_len)
		return 1;
	while(current[i])
	{
		if (ft_strcmp(current[i], saved[i]) != 0)
			return 1;
		i++;
	}
	return 0;
}
void main_init(t_info *info, char **envp)
{
	info->std_in_reserve = dup(STDIN_FILENO);
	info->std_out_reserve = dup(STDOUT_FILENO);
	info->old_dir = NULL;
	info->curr_dir = getcwd(NULL, 0);
	info->envp = copy_envp(envp);
	info->old_dir = get_env_value(envp, "OLDPWD");
	info->exit_status = 0;
}

void temp_print_info(t_info *info)
{
	printf("Input: %s\n", info->input ? info->input : "(null)");
	printf("Exit Status: %d\n", info->exit_status);
	printf("Environment Variables:\n");
	if (info->envp) {
		for (int i = 0; info->envp[i]; i++) {
			printf("  [%d]: %s\n", i, info->envp[i]);
		}
	} else {
		printf("  (null)\n");
	}
	printf("Interactive Mode: %d\n", info->interactive);
	printf("Old Directory: %s\n", info->old_dir ? info->old_dir : "(null)");
	printf("Current Directory: %s\n", info->curr_dir ? info->curr_dir : "(null)");
	printf("Standard Input Reserve FD: %d\n", info->std_in_reserve);
	printf("Standard Output Reserve FD: %d\n", info->std_out_reserve);
}

int main(int argc, char **argv, char **envp)
{
	t_info *info;
	info = malloc(sizeof(t_info));
	if(!info)
	{
		printf("Failed to allocate memory for info");
		return 1;
	}
	//	TODO: what are conditions to launch int/non-int mode, error cases?
	main_init(info, envp);
	temp_print_info(info);
	envp_compare(info->envp, envp);

	return 0;
}