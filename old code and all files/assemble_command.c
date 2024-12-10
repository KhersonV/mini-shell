
#include "l_p.h"
#include <unistd.h>
#include <stdlib.h>

int check_exp_var_chars(char c)
{
	return (c >= 'a' && c <= 'z')
	|| (c >= 'A' && c <= 'Z')
	|| (c >= '0' && c <= '9')
	|| (c == '_');
}

char *str_trim(char *s, int start, int end)
{
	int i = 0;
	char *out = (char *)malloc(sizeof(char) * (end - start) + 1);
	while(start < end)
	{
		out[i] = s[start];
		i++;
		start++;
	}
	out[i] = '\0';
	return out;

}

void open_expansion(char *s, char **envp)
{
	int start;
	int end;
	char *out;
	int i = 0;
	while(s[i])
	{
		if(s[i] == '$')
		{
			i++;
			if(s[i] == '_' || ((s[i] >= 'a' && s[i] <= 'z') || s[i] >= 'A' && s[i] <= 'Z'))
			{
				start = i;
				while(check_exp_var_chars(s[i]))
				{
					i++;
				}
				end = i;
				out = str_trim(s, start, end);
				printf("%s\n", out);
				start = end;
				continue;
			}
		}
		i++;
	}
}

int	main(int argc, char **argv, char **envp)
{

	// char input[] = "echo \'hello\'	>> file.txt | cat << input.txt | ls >> out.txt | grep 'hi' ";
	//char input[] = "echo 'test' > output.txt | cat";
	char intput[] = "abc$home $home2";

	open_expansion(intput, envp);

	// t_tree *root;
	// Node *output;
	// root = tokenize(input);
	// remove_spaces(&root);
	// adjusting_token_tree(&root);


	// output = parse_tokens(root);
	// // printf("after");
	// print_tokens(root);
	// printf("------\n");
	// print_ast(output, 1);

	return (0);
}