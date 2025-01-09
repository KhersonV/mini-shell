#include "../../include/minishell.h"


static int is_valid_var_char(char c)
{
	return (ft_isalnum((unsigned char)c) || c == '_');
}

static char *read_special_var(const char *input, int *consumed)
{
	if (input[1] == '?') {
		*consumed = 2;
		return (ft_strdup("?"));
	}
	return (NULL);
}

static char *read_variable_name(const char *input, int *consumed)
{
	char var_buf[256];
	int var_idx = 0;
	int i;

	i = 1;
	while (input[i] && is_valid_var_char(input[i])) {
		if (var_idx < 255) {
			var_buf[var_idx++] = input[i];
		}
		i++;
	}
	var_buf[var_idx] = '\0';
	*consumed = i;
	if (var_idx == 0) {
		return (ft_strdup("$"));
	}
	return (ft_strdup(var_buf));
}

char *read_var_name(const char *input, int *consumed)
{
	char *special_var;

	if (input[0] != '$') {
		*consumed = 0;
		return NULL;
	}
	special_var = read_special_var(input, consumed);
	if (special_var) {
		return special_var;
	}
	return (read_variable_name(input, consumed));
}
