

#include "../../include/minishell.h"



char *expand_variable(char *var_name, t_info  *info)
{
	
	
	if (strcmp(var_name, "?") == 0)
		return ft_itoa(info->exit_status);
	else
	{
		char *val = getenv(var_name);
		if (val == NULL)
			return "";
		return val;
	}
}

char *read_var_name(char **str)
{
	char var_buf[256];
	int idx = 0;

	while (**str != '\0' && (isalnum((unsigned char)**str) || **str == '_'))
	{
		var_buf[idx++] = **str;
		(*str)++;
		if (idx >= 255) break;
	}
	var_buf[idx] = '\0';

	return strdup(var_buf);
}

void parse_dollar(char **str, char *result, int *rindex, int max_len, t_info *info)
{
	if (**str == '\0')
	{
		if (*rindex < max_len - 1) {
			result[*rindex] = '$';
			(*rindex)++;
		}
		return;
	}
	if (**str == '?')
	{
		(*str)++;
		char *val = expand_variable("?", info);
		int len = strlen(val);
		if (*rindex + len < max_len)
		{
			strcpy(&result[*rindex], val);
			*rindex += len;
		}
		return;
	}
	if (isalnum((unsigned char)**str) || **str == '_')
	{
		char *var_name = read_var_name(str);

		char *val = expand_variable(var_name, info);
		free(var_name);

		int len = strlen(val);
		if (*rindex + len < max_len)
		{
			strcpy(&result[*rindex], val);
			*rindex += len;
		}
		return;
	}
	else
	{
		if (*rindex < max_len - 1) {
			result[*rindex] = '$';
			(*rindex)++;
		}
	}
}

void append_char(char *result, int *rindex, char c, int max_len)
{
	if (*rindex < max_len - 1)
	{
		result[*rindex] = c;
		(*rindex)++;
	}

}

char *expand_string(char *input, t_info *info)
{
	char result[1024];
	int rindex = 0;
	char *ptr = input;

	while (*ptr != '\0')
	{
		if (*ptr == '$')
		{
			ptr++;
			parse_dollar(&ptr, result, &rindex, sizeof(result), info);
		}
		else
		{
			append_char(result, &rindex, *ptr, sizeof(result));
			ptr++;
		}
	}
	if (rindex < (int)sizeof(result))
		result[rindex] = '\0';
	else
		result[sizeof(result) - 1] = '\0';
	return strdup(result);
}

void expansion(t_token **tokens, t_info *info)
{
	t_token *curr = *tokens;
	while (curr)
	{
		if (curr->type == TOKEN_WORD 
		|| curr->type == TOKEN_VAR 
		|| curr->type == TOKEN_EXP_FIELD 
		|| curr->type == TOKEN_EXIT_STATUS)
		{
			char *new_str = expand_string(curr->str, info);
			free(curr->str);
			curr->str = new_str;
		}
		curr = curr->next;
	}
}