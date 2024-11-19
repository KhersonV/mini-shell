
#include "../minishell.h"

/*
 TODO: add comparing envp's arrras function +
 TODO: add free two dim array function (or find it) +
*/


void free_info_struct(t_info *info);
int	ft_strcmp(const char *s1, const char *s2);
t_tree	*tokenize(char *s);
void	temp_print_tokens(t_tree *node);


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
	if (info->envp)
	{
		for (int i = 0; info->envp[i]; i++) {
			printf("  [%d]: %s\n", i, info->envp[i]);
		}
	} else
	{
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
	t_tree *root;
	// char input[] = ""; +
	// char input[] = "| < << > >>"; +
	// char input[] = "$var1 $var2 $var_3"; +
	// char input[] = "echo$USER|grep$HOME"; +
	// char input[] = "$9invalid $%notvar"; +
	// char input[] = "'It''s a test' \"She said, \\\"Hello\\\"\""; -
	/**
	 * Token: It's a test, Type: FIELD
	Token: [], Type: TOKEN_SPACE
	Token: She said, \"Hello\", Type: EXP_FIELD
	*/
	// char input[] = "echo 'This is unclosed string"; +
	// char input[] = "cmd1||cmd2"; +
	// char input[] = "cat>file.txt";
	// char input[] = "filename_with-special.characters"; +
	// char input[] = "echo    'hello' \t\t   world"; +
	// char input[] = "echo 'line1\nline2'";
	// char input[] = "$VAR123abc$VAR_456$VAR$"; -
	/**
	 * Token: VAR123abc, Type: VAR
		Token: VAR_456, Type: VAR
		Token: VAR, Type: VAR
		Token: $, Type: WORD
	*/
	// char input[] = "echo >| file"; +
	// char input[] = "\"This is 'nested' quotes\"";  -
	/**
	 * Token: This is 'nested' quotes, Type: EXP_FIELD
	*/
	// char input[] = "command >"; +
	// char input[] = "> output.txt"; +
	// char input[] = "echo \"Hello\"World'!' $USER"; +
	// char input[] = "echo file@name#with$pecial%chars"; -
	/*
	Token: echo, Type: WORD
	Token: [], Type: TOKEN_SPACE
	Token: file@name#with, Type: WORD
	Token: pecial%chars, Type: WORD
	*/
	// char input[] = "echo $(date)"; -
	/*
	Token: echo, Type: WORD
	Token: [], Type: TOKEN_SPACE
	Token: $(date), Type: WORD
	*/
	// char input[] = "echoHello$USER"; +
	// char input[] = "    echo    'test'   "; +
	// char input[] = "$VAR|$ANOTHER_VAR>$OUTPUT"; +
	// char input[] = "$"; -
	/*
	Token: $, Type: WORD
	*/
	// char input[] = "$VAR!$VAR?$$"; -
	/*
	Token: VAR, Type: VAR
	Token: !, Type: WORD
	Token: VAR, Type: VAR
	Token: ?, Type: WORD
	Token: $, Type: WORD
	*/
	// char input[] = "\"Double 'Single' Double\" 'Single \"Double\" Single'";
	/*
	Token: Double 'Single' Double, Type: EXP_FIELD
	Token: [], Type: TOKEN_SPACE
	Token: Single "Double" Single, Type: FIELD
	*/
	// char input[] = "echo backslash\\test"; // +

	char input[] = "export VAR=value";

	info = malloc(sizeof(t_info));
	if(!info)
	{
		printf("Failed to allocate memory for info");
		return 1;
	}
	//	TODO: what are conditions to launch int/non-int mode, error cases?
	main_init(info, envp);
	envp_compare(info->envp, envp);
	root = tokenize(input);
	// printf("before:\n");
	// temp_print_tokens(root);
	// printf("\nafter \n:");
	// remove_spaces(&root);
	// adjusting_token_tree(&root);

	temp_print_tokens(root);



	return 0;
}



	/* more tests
	*/
	// char input[] = "echo hello world"; + +
	// char input[] = "ls -la /home/user"; ++
	// char input[] = "cat file.txt > output.txt";
	// char input[] = "grep 'pattern' < input.txt";
	// char input[] = "echo $HOME | wc -l";
	/*
	:Token: echo, Type: COMMAND
	Token: HOME, Type: VAR - should be arg.
	Token: |, Type: PIPE
	Token: wc, Type: COMMAND
	Token: -l, Type: ARGUMENT
	*/
	// char input[] = "sort < unsorted.txt > sorted.txt";
	// char input[] = "command arg1 arg2 | another_command arg3";
	// char input[] = "echo 'Hello World' >> output.txt";
	// char input[] = "echo $USER $PATH";
	/*
	same
	*/
	// char input[] = "rm -rf /some/directory";
	// char input[] = "mkdir new_folder && cd new_folder";
	/*
	:Token: mkdir, Type: COMMAND
		Token: new_folder, Type: ARGUMENT
		Token: &&, Type: ARGUMENT - doesn't understand &&, but fine
		Token: cd, Type: ARGUMENT
		Token: new_folder, Type: ARGUMENT
	*/
	// char input[] = "echo \"Nested 'quotes' test\""; -
	/*
	Token: echo, Type: COMMAND
	Token: Nested 'quotes' test, Type: EXP_FIELD - should be arg
	*/
	// char input[] = "ps aux | grep 'process_name'";
	// char input[] = "echo \"User: $USER, Path: $PATH\""; //same, exp_field to arg