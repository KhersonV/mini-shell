
#include "../../include/minishell.h"

/*
	/bin/echo $"42$"
	/bin/echo $USER'$USER'text oui oui     oui  oui $USER oui      $USER ''
	/bin/echo '' ""


*/

// static int is_empty_string(const char *s)
// {
//     return (s == NULL || s[0] == '\0');
// }

// static t_token *remove_token(t_token *head, t_token *del)
// {
//     if (!del) return head;

//     t_token *prev = del->prev;
//     t_token *next = del->next;

//     // Если удаляем head
//     if (head == del)
//         head = next;

//     // Связываем prev->next = next
//     if (prev)
//         prev->next = next;
//     // Связываем next->prev = prev
//     if (next)
//         next->prev = prev;

//     // Освобождаем память
//     free(del->str);
//     free(del);

//     return head;
// }


// void fix_empty_arguments(t_token **head_ref)
// {
//     t_token *curr = *head_ref;
//     t_token *args[256]; // Массив для хранения указателей на аргументы (упрощенно)
//     int arg_idx = 0;

//     while (curr)
//     {
//         // Если это PIPE или конец, мы «завершаем» предыдущую команду
//         if (curr->type == TOKEN_PIPE || curr->next == NULL)
//         {
//             // Если curr->next==NULL, значит это конец списка — 
//             // но нужно учесть и этот токен, если он не PIPE
//             int end_is_command = 0;
//             if (curr->type != TOKEN_PIPE && curr->next == NULL)
//             {
//                 // Возможно, это ARGUMENT тоже
//                 if (curr->type == TOKEN_ARGUMENT)
//                 {
//                     // Добавим в args
//                     if (arg_idx < 256)
//                         args[arg_idx++] = curr;
//                 }
//                 end_is_command = 1;
//             }

//             // Теперь у нас есть массив args[0..arg_idx-1].
//             // Применим логику:
//             if (arg_idx == 1) 
//             {
//                 // Если ровно 1 аргумент
//                 t_token *only_arg = args[0];
//                 if (is_empty_string(only_arg->str))
//                 {
//                     // Удаляем этот токен
//                     *head_ref = remove_token(*head_ref, only_arg);
//                 }
//             }
//             else if (arg_idx > 1)
//             {
//                 // Если аргументов несколько
//                 for (int k = 0; k < arg_idx; k++)
//                 {
//                     if (is_empty_string(args[k]->str))
//                     {
//                         // Заменяем на " "
//                         free(args[k]->str);
//                         args[k]->str = strdup(" ");
//                     }
//                 }
//             }

//             // Подготовиться к обработке следующей команды
//             arg_idx = 0;
//             // Если этот токен был PIPE, следующая команда начнется после него
//             // Если это конец списка, мы закончим цикл
//             curr = curr->next;
//             continue;
//         }

//         // Иначе, если это не PIPE, мы проверяем:
//         if (curr->type == TOKEN_COMMAND)
//         {
//             // Начало новой команды, сбрасываем массив аргументов
//             arg_idx = 0;
//         }
//         else if (curr->type == TOKEN_ARGUMENT)
//         {
//             // Сохраняем указатель в массив
//             if (arg_idx < 256) // чисто чтобы избежать переполнения
//                 args[arg_idx++] = curr;
//         }

//         curr = curr->next;
//     }
// }


char *expand_variable(char *var_name, t_info *info);
static char* expand_dollar(const char *input, int *consumed, t_info *info);

char *ft_expand_variable(char *var_name, t_info *info)
{
    // if (ft_strcmp(var_name, "?") == 0)
    // {
    //     char *exit_str = ft_itoa(info->exit_status);
    //     return exit_str;
    // }
    // else if (ft_strcmp(var_name, "$") == 0)
    // {
    //     char *pid_str = ft_itoa(getpid());
    //     return pid_str;
    // }
    // else
    // {
    char *val = get_env_value(info, var_name);
    if (val == NULL)
        return ft_strdup("");
    return ft_strdup(val);
    // }
}





// TODO : echo $$HOME,  $$ check.
void expansion(t_token **tokens, t_info *info);

static int is_space_char(char c)
{
	return (c == ' ' || (c >= 9 && c <= 13));
}

static int is_operator_char(char c)
{
	return (c == '|' || c == '<' || c == '>');
}

int	ft_isalnum(int c)
{
	if ((c >= '0' && c <= '9')
		|| ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')))
		return (1);
	return (0);
}


static char* read_var_name(const char *input, int *consumed)
{
    int i = 0;
    if (input[i] == '$') {
        i++;
        if (input[i] == '?') {
            i++;
            *consumed = i;
            return strdup("?");
        }

        char var_buf[256];
        int var_idx = 0;
        while (input[i] && (isalnum((unsigned char)input[i]) || input[i] == '_')) {
            if (var_idx < 255) {
                var_buf[var_idx++] = input[i];
            }
            i++;
        }
        var_buf[var_idx] = '\0';
        *consumed = i; 
        if (var_idx == 0) { 
            return strdup("$"); 
        }
        return strdup(var_buf);
    }
    *consumed = 0;
    return NULL;
}


char	*print_token(int current_token)
{
	switch (current_token)
	{
		case TOKEN_SPACE:
			return ("TOKEN_SPACE");
		case TOKEN_FILE:
			return ("TOKEN_FILE");
		case TOKEN_HEREDOC:
			return ("TOKEN_HEREDOC");
		case TOKEN_COMMAND:
			return ("TOKEN_COMMAND");
		case TOKEN_ARGUMENT:
			return("TOKEN_ARGUMENT");
		case TOKEN_REDIRECT_APPEND:
			return ("TOKEN_REDIRECT_APPEND");
		case TOKEN_PIPE:
			return ("TOKEN_PIPE");
		case TOKEN_REDIRECT_OUT:
			return ("TOKEN_REDIRECT_OUT");
		case TOKEN_REDIRECT_IN:
			return ("TOKEN_REDIRECT_IN");
		case TOKEN_WORD:
			return ("TOKEN_WORD");
		case TOKEN_VAR:
			return ("TOKEN_VAR");
		case TOKEN_EXP_FIELD:
			return ("TOKEN_EXP_FIELD");
		case TOKEN_FIELD:
			return ("TOKEN_FIELD");
		case TOKEN_HEREDOC_MARKER:
			return ("TOKEN_HEREDOC_MARKER");
		case TOKEN_EXIT_STATUS:
			return ("TOKEN_EXIT_STATUS");
		default:
			return ("UNKNOWN_TYPE");
	}
}

static int append_char_to_buf(char *buf, int *idx, int buf_size, char c)
{
    if (*idx >= buf_size - 1) {
        return -1; 
    }
    buf[(*idx)++] = c;
    return 0;
}

static int read_single_quoted(const char *input, char *buf, int *buf_index, int buf_size)
{
    int i = 1; 
    while (input[i] && input[i] != '\'') {
        if (append_char_to_buf(buf, buf_index, buf_size, input[i]) < 0) {
            fprintf(stderr, "Buffer overflow in single quotes\n");
            return i;
        }
        i++;
    }
    if (input[i] == '\'') {
        i++;
    }
    return i; 
}


t_token	*create_token_node(char *name, int type)
{
	t_token	*new_node;

	new_node = (t_token *)malloc(sizeof(t_token));
	if (!new_node)
		return (NULL);
	new_node->str = ft_strdup(name);
	new_node->type = type;
	new_node->next = NULL;
	new_node->prev = NULL;
	return (new_node);
}

static char* read_dollar_quoted(const char *input, int *consumed, t_info *info)
{
    // Ожидаем, что input[0] == '$' и input[1] == ''' или '"'
    char quote = input[1]; 
    int i = 2; // Пропускаем символы $ и (') или (")
    
    char buf[1024];
    int buf_index = 0;
    int stop = 0; // Флаг, если случилось что-то, требующее выхода

    if (quote == '\'')  // $'...'
    {
        // Просто копируем до следующей одинарной кавычки
        while (input[i] && input[i] != '\'' && !stop)
        {
            // НЕ раскрываем переменные, не обрабатываем бэкслэши
            if (append_char_to_buf(buf, &buf_index, 1024, input[i]) < 0)
            {
                fprintf(stderr, "Buffer overflow in $'...'\n");
                stop = 1;
                break;
            }
            i++;
        }
        // Если нашли закрывающую кавычку
        if (!stop && input[i] == '\'')
            i++;
    }
    else if (quote == '"') // $"..."
    {
        // Аналогично double quotes, но с раскрытием $ и экранированием
        while (input[i] && input[i] != '"' && !stop)
        {
            if (input[i] == '\\')
            {
                i++;
                // Если вдруг строка кончилась на бэкслэше
                if (!input[i]) 
                    break;

                // Экранируем ", $,
                if (strchr("\"$\\", input[i]))
                {
                    if (append_char_to_buf(buf, &buf_index, 1024, input[i]) < 0)
                    {
                        fprintf(stderr, "Buffer overflow in $\"...\"\n");
                        stop = 1;
                        break;
                    }
                    i++;
                }
                else
                {
                    // Пишем '\' + текущий символ
                    if (append_char_to_buf(buf, &buf_index, 1024, '\\') < 0)
                    {
                        fprintf(stderr, "Buffer overflow in $\"...\"\n");
                        stop = 1;
                        break;
                    }
                    if (append_char_to_buf(buf, &buf_index, 1024, input[i]) < 0)
                    {
                        fprintf(stderr, "Buffer overflow in $\"...\"\n");
                        stop = 1;
                        break;
                    }
                    i++;
                }
            }
            else if (input[i] == '$')
            {
                // Раскрытие переменной внутри $"..."
                int var_consumed = 0;
                char *expanded = expand_dollar(&input[i], &var_consumed, info);
				printf("last dollar sign is - %s\n", expanded);
                if (!expanded)
                {
                    // На случай, если expand_dollar вернёт NULL —  
                    // вы сами решаете, как обработать
                    fprintf(stderr, "expand_dollar returned NULL\n");
                    stop = 1;
                    break;
                }

                // Копируем expanded
                for (int k = 0; expanded[k] != '\0'; k++)
                {
                    if (append_char_to_buf(buf, &buf_index, 1024, expanded[k]) < 0)
                    {
                        fprintf(stderr, "Buffer overflow in $\"...\" expand\n");
                        free(expanded);
                        stop = 1;
                        break;
                    }
                }
                free(expanded);

                // Если внутри цикла уже выставили stop = 1, тоже прерываем
                if (stop)
                    break;

                i += var_consumed;
            }
            else
            {
                // Обычный символ
                if (append_char_to_buf(buf, &buf_index, 1024, input[i]) < 0)
                {
                    fprintf(stderr, "Buffer overflow in $\"...\"\n");
                    stop = 1;
                    break;
                }
                i++;
            }
        }

        // Если цикл закончился не из-за stop, 
        // но из-за input[i] == '"' — "проглатываем" закрывающую кавычку
        if (!stop && input[i] == '"')
            i++;
    }

    // Завершаем строку в буфере
    buf[buf_index] = '\0';

    // Если это $'...', возможно стоит проглотить закрывающую кавычку (вне цикла)
    // но мы уже сделали выше для одинарной кавычки:
    // if (!stop && input[i] == '\'') i++;

    // Запоминаем, сколько всего «съели»
    *consumed = i;

    // Возвращаем malloc'нутую копию
    // (даже если stop == 1, вернём то, что удалось записать; 
    //  или вы можете вернуть пустую строку / NULL)
    return strdup(buf);
}


static char* expand_dollar(const char *input, int *consumed, t_info *info)
{
	
if (input[1] == '\'' || input[1] == '"')
    {
        // --- ДОБАВЛЕНА проверка --- 
        // Смотрим, что в input[2]? 
        // Если там нет "имени" переменной (а именно нет букв, цифр, '_', '?' и т.д.), 
        // то скорее всего это просто `$"` (или `$'`) без имени -> вернём буквально "$".
        
        char next_char = input[2];
        
        // Можно уточнить, что вы считаете "признаком содержимого":
        // ни пробел, ни конец строки, ни повторная кавычка, 
        // ни оператор, ... 
        // Но чаще всего хватает "не буква/цифра/'?'/'_'"
        
        if (next_char == '\0'                // строка кончилась
         || is_space_char(next_char)         // пробел/таб
         || is_operator_char(next_char)      // |, <, >
         || next_char == '\''                // $"''
         || next_char == '"'                 // $""
         )
        {
            // В этих случаях считаем, что НЕТ никакого имени/содержимого
            // => вернём просто "$"
            *consumed = 1; 
            return strdup("$");
        }
        else
        {
            // Если после кавычки есть хоть что-то "осмысленное" (например, H, 4, и т.д.),
            // значит это действительно $'...' или $"...' 
            // => вызываем уже знакомую функцию
            char *res = read_dollar_quoted(input, consumed, info);
            return res; // готовая строка
        }
    }

    int var_consumed = 0;
    char *var_name = read_var_name(input, &var_consumed);
    if (!var_name) {
        *consumed = 0;
        return strdup("");
    }

	*consumed = var_consumed;


	if (strcmp(var_name, "$") == 0) 
	{
        // free(var_name);
        return strdup("$");
    }

	if (strcmp(var_name, "?") == 0) {
        // free(var_name);
        char *exit_str = ft_itoa(info->exit_status);
        return exit_str;
    }

	char *expanded = ft_expand_variable(var_name, info);
    // free(var_name);
    return expanded; // уже malloc'нута

	return var_name;
}

t_token	*add_token(t_token *node, char *name, int type)
{
	t_token	*new_node;
	t_token	*curr;

	new_node = create_token_node(name, type);
	if (!new_node)
		return (NULL);
	if (!node)
		return (new_node);
	curr = node;
	while (curr->next)
	{
		curr = curr->next;
	}
	curr->next = new_node;
	new_node->prev = curr;
	return (node);
}

void	flush_buf_if_needed(t_token **curr, char *buf, int *buf_index)
{
	if(*buf_index > 0)
	{
		buf[*buf_index] = '\0';
		*curr = add_token(*curr, buf, TOKEN_WORD);
		*buf_index = 0;
	}
}

int is_quotes_closed(const char *start)
{
	char quote = *start;
	int i = 1;
	while (start[i]) {
		if (start[i] == quote)
			return 1;
		i++;
	}
	return 0;
}


t_token *add_operator_token(t_token *curr, char current_char, char next_char, int *i)
{
	if (current_char == '|')
		curr = add_token(curr, "|", TOKEN_PIPE);
	else if (current_char == '<')
	{
		if (next_char == '<')
		{
			curr = add_token(curr, "<<", TOKEN_HEREDOC);
			(*i)++;
		}
		else
			curr = add_token(curr, "<", TOKEN_REDIRECT_IN);
	}
	else if (current_char == '>')
	{
		if (next_char == '>')
		{
			curr = add_token(curr, ">>", TOKEN_REDIRECT_APPEND);
			(*i)++;
		}
		else
			curr = add_token(curr, ">", TOKEN_REDIRECT_OUT);
	}
	// else if (current_char == ' ' || (current_char >= 9 && current_char <= 13))
	// 	curr = add_token(curr, "[]", TOKEN_SPACE);
	return curr;
}


static int read_double_quoted(const char *input, char *buf, int *buf_index, int buf_size, t_info *info)
{
	int i = 1;
	while(input[i] && input[i] != '"')
	{
		if (input[i] == '\\')
		{
			i++;
			if(!input[i])
			{
				break;
			}
			if(input[i] == '"' || input[i] == '$' || input[i] == '\\')
			{
				if(append_char_to_buf(buf, buf_index, buf_size, input[i]) < 0) 
				{
					fprintf(stderr, "Buffer overflow in double quotes\n");
                    return i;
				}
				i++;
			} else {
                if (append_char_to_buf(buf, buf_index, buf_size, '\\') < 0) {
                    fprintf(stderr, "Buffer overflow in double quotes\n");
                    return i;
                }
                if (append_char_to_buf(buf, buf_index, buf_size, input[i]) < 0) {
                    fprintf(stderr, "Buffer overflow in double quotes\n");
                    return i;
                }
                i++;
			}
		} else if (input[i] == '$') 
		{
			int consumed = 0;
			char *expanded = expand_dollar(&input[i], &consumed, info);


			for(int k = 0; expanded[k] != '\0'; k++)
			{
				if (append_char_to_buf(buf, buf_index, buf_size, expanded[k]) < 0) {
                    fprintf(stderr, "Buffer overflow in double quotes\n");
                    // free(expanded);
                    return i + consumed;
                }
				// printf("buffer - %s\n", buf);
			}
			// free(expanded);
			i += consumed;
		} 
		else
		{
            if (append_char_to_buf(buf, buf_index, buf_size, input[i]) < 0) {
                fprintf(stderr, "Buffer overflow in double quotes\n");
                return i;
            }
            i++;
        }
    }
	// printf("last char in field - [%c]\n", input[i]); - []
    if (input[i] == '"') {
        i++; 
    }
    return i;
}

static int read_unquoted(const char *input, char *buf, int *buf_index, int buf_size, t_info *info)
{
    int i = 0;
    while (input[i] != '\0') {
        if (is_space_char(input[i]) || is_operator_char(input[i])) {
            break;
        }
        if (input[i] == '\'' || input[i] == '"') {
            break;
        }
        if (input[i] == '$') {
            int consumed = 0;
            char *expanded = expand_dollar(&input[i], &consumed, info);
            for (int k = 0; expanded[k] != '\0'; k++) {
                if (append_char_to_buf(buf, buf_index, buf_size, expanded[k]) < 0) {
                    fprintf(stderr, "Buffer overflow in unquoted\n");
                    // free(expanded);
                    return i + consumed;
                }
            }
            // free(expanded);
            i += consumed;
            continue;
        }
        if (input[i] == '\\') {
            i++;
            if (!input[i]) break;

            if (strchr("$\\\"\'", input[i])) {
                if (append_char_to_buf(buf, buf_index, buf_size, input[i]) < 0) {
                    fprintf(stderr, "Buffer overflow in unquoted (backslash)\n");
                    return i;
                }
                i++;
            } else {
                // Иначе пишем сам бэкслэш + символ
                if (append_char_to_buf(buf, buf_index, buf_size, '\\') < 0) {
                    fprintf(stderr, "Buffer overflow in unquoted (backslash)\n");
                    return i;
                }
                // второй символ
                if (append_char_to_buf(buf, buf_index, buf_size, input[i]) < 0) {
                    fprintf(stderr, "Buffer overflow in unquoted (backslash)\n");
                    return i;
                }
                i++;
            }
            continue;
        }

        if (append_char_to_buf(buf, buf_index, buf_size, input[i]) < 0) {
            fprintf(stderr, "Buffer overflow in unquoted\n");
            return i;
        }
        i++;
    }
    return i;
}



t_token *tokenizer(char *user_input, t_info *info)
{
	t_token *head = NULL;

	char buf[1024];
	int buf_index = 0;

	int i = 0;

	while(user_input[i] != '\0')
	{
		// printf("next char - [%c]\n", user_input[i]);
		if(is_space_char(user_input[i]))
		{
			flush_buf_if_needed(&head, buf, &buf_index);
			i++;
			continue;
		}
		if(is_operator_char(user_input[i]))
		{
			flush_buf_if_needed(&head, buf, &buf_index);
			head = add_operator_token(head, user_input[i], user_input[i+1], &i);
			i++;
			continue;
		}
		if(user_input[i] == '\'')
		{
			int consumed = read_single_quoted(&user_input[i], buf, &buf_index, 1024);
			i += consumed;
			
			continue;
		}
		if(user_input[i] == '\"')
		{
			int consumed = read_double_quoted(&user_input[i], buf, &buf_index, 1024, info);
			i += consumed;
			continue;
		}
		
		int consumed = read_unquoted(&user_input[i], buf, &buf_index, 1024, info);
		
        i += consumed;
	}

	flush_buf_if_needed(&head, buf, &buf_index);

	// printf("final buf = %s", buf);
	return head;
}


void adjusting_token_tree(t_token **tree)
{
	t_token *curr;
	int command_found;

	curr = *tree;
	command_found = 0;
	while (curr != NULL)
	{
		if (curr->type == TOKEN_PIPE)
		{
			command_found = 0;
		}
		if (!command_found && (curr->type == TOKEN_WORD || curr->type == TOKEN_EXP_FIELD || curr->type == TOKEN_FIELD || curr->type == TOKEN_VAR))
		{
			curr->type = TOKEN_COMMAND;
			command_found = 1;
		}
		else if (command_found && (curr->type == TOKEN_WORD || curr->type == TOKEN_FIELD ||
								   curr->type == TOKEN_EXP_FIELD || curr->type == TOKEN_VAR
								   || curr->type == TOKEN_EXIT_STATUS))
		{
			curr->type = TOKEN_ARGUMENT;
		}
		if (curr->type == TOKEN_REDIRECT_IN || curr->type == TOKEN_REDIRECT_OUT ||
			curr->type == TOKEN_REDIRECT_APPEND)
		{
			if (curr->next != NULL)
				curr->next->type = TOKEN_FILE;
		}
		else if (curr->type == TOKEN_HEREDOC)
		{
			if (curr->next != NULL)
				curr->next->type = TOKEN_HEREDOC_MARKER;
		}
		curr = curr->next;
	}
}

// int main()
// {
// 	t_token *test;
// 	t_token *test2;
// 	t_info *info;
// 	// char input[] = "echo Hello world > out.txt | grep 'pattern' < in.txt";
// 	// char input[] = "echo 'static text' \"$DYNAMIC_VAR\" $USER";
// 	// char input[] = "echo Hello | grep 'pattern' > out.txt";
// 	// char input[] = "cat $HOME.txt | echo \"$HOMEsomeworkds\" ";
// 	char input[] = "echo $HOME$? >> out.txt | echo $?\"42\"";

// 	// char input[] = "env VAR=HELLO";

// 	printf("Input command: %s\n", input);
// 	test = tokenize(input);

// 	test2 = tokenizer(input, info);

// 	printf("\nNew tokenizer:\n");
// 	temp_print_tokens(test2);
// 	printf("---------\n");


// 	// printf("\nTokens:\n");
// 	// temp_print_tokens(test);

// 	// remove_space_tokens(&test);

// 	// expansion(&test,info);

// 	// printf("\nTokens after expansion:\n");
// 	// temp_print_tokens(test);

// 	adjusting_token_tree(&test);

// 	printf("\nTokens after adjustment:\n");
// 	temp_print_tokens(test);

// 	return 0;
// }
