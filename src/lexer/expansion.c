

#include "../../include/minishell.h"

/*
old version

char *expand_variable(char *var_name, t_info  *info)
{
	if (ft_strcmp(var_name, "?") == 0)
	{
		return ft_itoa(info->exit_status);
	}
	// else if (ft_strcmp(var_name, "$$") == 0)
	// {
	// 	char *pid_str = malloc(12); // Достаточно места для PID
	// 	if (!pid_str)
	// 	{
	// 		fprintf(stderr, "Memory allocation error\n");
	// 		exit(1);
	// 	}
	// 	snprintf(pid_str, 12, "%d", getpid());
	// 	return pid_str;
	// }
	else
	{
		char *val = getenv(var_name);
		if (val == NULL)
			return "";
		return val;
	}
}

*/

char *expand_variable(char *var_name, t_info *info)
{
    if (ft_strcmp(var_name, "?") == 0)
    {
        char *exit_str = ft_itoa(info->exit_status);
        return exit_str;
    }
    else if (ft_strcmp(var_name, "$") == 0)
    {
        char *pid_str = ft_itoa(getpid());
        return pid_str;
    }
    else
    {
        char *val = get_env_value(info, var_name);
        if (val == NULL)
            return ft_strdup("");
        return ft_strdup(val);
    }
}


char *read_var_name(char **str)
{
	char var_buf[256];
	int idx = 0;

	while (**str != '\0' && (ft_isalnum((unsigned char)**str) || **str == '_'))
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
	if (ft_isalnum((unsigned char)**str) || **str == '_')
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


/*
staraja versija



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
*/


char *expand_string(char *input, t_info *info)
{
    char result[1024];
    int rindex = 0;
    char *ptr = input;

    while (*ptr != '\0')
    {
        // Проверяем, находится ли '~' в начале слова
        if (*ptr == '~' && (rindex == 0 || result[rindex - 1] == ' ') && (ptr[1] == '/' || ptr[1] == '\0'))
        {
            char *home = get_env_value(info, "HOME");
            if (!home)
            {
                home = info->home; // Используем резервное значение
            }

            if (home)
            {
                int len = strlen(home);
                if (rindex + len < (int)sizeof(result))
                {
                    strcpy(&result[rindex], home);
                    rindex += len;
                }
                // Не освобождайте home, если оно ссылается на info->home
                if (get_env_value(info, "HOME"))
                    free(home);
                ptr++; // Пропустить '~'
                if (*ptr == '/')
                {
                    if (rindex < (int)sizeof(result) - 1)
                    {
                        result[rindex++] = '/';
                        ptr++;
                    }
                }
                continue;
            }
        }
        else if (*ptr == '$')
        {
            ptr++;
            parse_dollar(&ptr, result, &rindex, sizeof(result), info);
            continue;
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
    return ft_strdup(result);
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

/*

>> '$USER'
cat $USER
/bin/rm -f '$USER'

sig fault, check arguments that goes to execute

>> '$USER'
cat '$USER'
/bin/rm -f '$USER'

same. ask gpt about this command

echo '' -n
------
echo "" "" "" -n -n -n -n
-------
cat << $USER
why
not
$USER
-------
these 3 stdout errors
----------
cat << "$US"E"R"
because
we
love
bash
$USER

>> $HOME

>> "$H"OM"E"
cat OME
/bin/rm -f OME

>> "$USER'$USER'"
cat "$USER'$USER'"
/bin/rm -f "$USER'$USER'"

>> "$USER"
cat $USER
/bin/rm -f $USER

-------

ENV


'''''''''''''''' echo ok

echo "$"$'$'$"$"$"$"$'$'


export
----wrong stdout.


ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls

ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls | ls
?????


----------------------------
probably very complicated, left untill end?

ls | cat << stop | grep "asd"
is this good
stop

ls | cat << stop | ls -la | cat << stop1
12
32232
23
stop
awdaw
daswd
stop1

ls | cat << stop | ls -la | cat << stop1 | ls | cat << stop2 | ls -la | cat << stop3
$USER
ad
as $HOME
stop
awd
wf$PWDdqwdwqd
stop1
das
das
stop2
dsq
wd
wf$PWDdqwdwqd
stop3

ls | cat << stop | ls -la | cat << stop1 | ls | cat << stop2 | ls -la > out | cat << 'stop3'
$USER
ad
stop
dsa
stop1
sad
stop2
as $HOME
stop3
/bin/rm -f out

ls | cat << stop | ls -la | cat << stop1 | ls | cat << stop2 | ls -la > > out | cat << stop3
fe
wf
fwe
f
stop
dw
stop1
dwasd
stop2
cat Makefile | ls > out
cat out
/bin/rm -f out

ls | cat << stop | ls -la > out0| cat << stop1 | ls | cat << stop2 | ls -la >> out | cat << stop3
fe
wf
fwe
f
stop
dw
stop1
dw
stop2
e
wf
stop3
/bin/rm -f out
/bin/rm -f out0

---------
ls|cat Makefile|cat<<asd>out
$USER
asd
/bin/rm -f out

ls|cat Makefile|cat<<'asd'>out
$USER
asd
/bin/rm -f out

ls|cat Makefile|cat<<"asd">out
$USER
asd
/bin/rm -f out

pwd
ls|cat Makefile|>> out|cd ..
/bin/rm -f out
pwd

-------------

/bin/echo 42 > /dev/null > /dev/null > /dev/null > /dev/null > tmp_redir_out
cat tmp_redir_out
rm tmp_redir_out

/bin/echo 42 > tmp_redir_out > tmp_redir_out1 > tmp_redir_out2 > tmp_redir_out3
/bin/echo 0
cat tmp_redir_out
/bin/echo 1
cat tmp_redir_out1
/bin/echo 2
cat tmp_redir_out2
/bin/echo 3
cat tmp_redir_out3
rm tmp_redir_out tmp_redir_out1 tmp_redir_out2 tmp_redir_out3

/bin/echo 42 > /dev/null > tmp_redir_out
/bin/echo 2 >> /dev/null >> tmp_redir_out
cat tmp_redir_out
rm tmp_redir_out
-------

> out
/bin/rm -f out

< out
/bin/rm -f out

>> out
/bin/rm -f out
-------------

unset PATH
cd /bin
ls

unset PATH
cd /bin/../bin/
ls

unset PATH
echo $PATH
/bin/ls
echo 1
cd -
echo 42

cd ~/Desktop/
pwd

env -i ./minishell
cd /bin/
ls

touch tmp_x_file1
tmp_x_file1
echo $?
./tmp_x_file1
echo $?
export PATH=$HOME
echo $PATH
tmp_x_file1
echo $?
./tmp_x_file1
echo $?
unset PATH
tmp_x_file1
echo $?
./tmp_x_file1
echo $?
echo 42
/bin/rm -f tmp_x_file1

export test="arg1	arg2"
echo 'echo $1' > tmp_test_sh
bash tmp_test_sh $test
echo 'echo $2' > tmp_test_sh
bash tmp_test_sh $test
rm -f tmp_test_sh

echo "env | /usr/bin/wc -l" | env -i $MINISHELL_PATH"/"$EXECUTABLE
echo $?

echo "unset PATH" | env -i $MINISHELL_PATH"/"$EXECUTABLE
echo $?

---------------------------------------
cd ~/Desktop/
pwd

*/
