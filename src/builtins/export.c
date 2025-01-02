/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/10 14:25:29 by vmamoten          #+#    #+#             */
/*   Updated: 2025/01/02 13:37:56 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

void	sort_env(char **env)
{
	int		i;
	int		j;
	char	*tmp;

	i = 0;
	while (env[i])
	{
		j = i + 1;
		while (env[j])
		{
			if (ft_strcmp(env[i], env[j]) > 0)
			{
				tmp = env[i];
				env[i] = env[j];
				env[j] = tmp;
			}
			j++;
		}
		i++;
	}
}


int	is_numerick(const char *str)
{
	if (*str == '-' || *str == '+')
		str++;
	if (!*str)
		return (0);
	while (*str)
	{
		if (!ft_isdigit(*str))
			return (0);
		str++;
	}
	return (1);
}

void ft_export(char **args, t_info *info)
{
    char    **sorted_env;
    int     i;
    char    *key;
    char    *value;
    int     exit_status = 0;
    char    *equal_sign;
    int     j;

    // Если нет аргументов, вывести все переменные с "declare -x"
    if (!args[1])
    {
        sorted_env = copy_envp(info->envp);
        if (!sorted_env)
        {
            perror("minishell: export: failed to copy environment");
            info->exit_status = 1;
            return;
        }
        sort_env(sorted_env);
        i = 0;
        while (sorted_env[i])
        {
            equal_sign = ft_strchr(sorted_env[i], '=');
            if (equal_sign)
                printf("declare -x %.*s=\"%s\"\n", (int)(equal_sign - sorted_env[i]), sorted_env[i], equal_sign + 1);
            else
                printf("declare -x %s\n", sorted_env[i]);
            i++;
        }
        free_env(&(t_info){.envp = sorted_env});
        info->exit_status = 0;
        return;
    }

    // Обработка аргументов
    i = 1;
    while (args[i])
    {
        // Проверка на опции (аргументы, начинающиеся с '-')
        if (args[i][0] == '-' && args[i][1] != '\0')
        {
            // Итерируемся по каждому символу после '-'
            j = 1;
            while (args[i][j] != '\0')
            {
                if (args[i][j] != 'n' && args[i][j] != 'f' && args[i][j] != 'p')
                {
                    // Недопустимая опция найдена
                    ft_putstr_fd("minishell: export: -", 2);
                    ft_putchar_fd(args[i][j], 2);
                    ft_putstr_fd(": invalid option\n", 2);
                    ft_putstr_fd("export: usage: export [-nf] [name[=value] ...] or export -p\n", 2);
                    info->exit_status = 2;
                    return; // Прекращаем дальнейшую обработку
                }
                j++;
            }
            // Если все опции допустимы, можно реализовать их обработку здесь
            // В текущей реализации опции не обрабатываются, просто пропускаем
            i++;
            continue;
        }

        // Разделение аргумента на ключ и значение, если присутствует '='
        equal_sign = ft_strchr(args[i], '=');
        if (equal_sign)
        {
            key = ft_strndup(args[i], equal_sign - args[i]);
            value = ft_strdup(equal_sign + 1);
        }
        else
        {
            key = ft_strdup(args[i]);
            value = NULL;
        }

        // Проверка корректности ключа
        if (is_valid_env_key(key))
        {
            if (value)
                set_env(info, key, value);
            if (exit_status != 2)
                info->exit_status = 0;
        }
        else
        {
            ft_putstr_fd("minishell: export: `", 2);
            ft_putstr_fd(args[i], 2);
            ft_putstr_fd("': not a valid identifier\n", 2);
            if (exit_status != 2)
                exit_status = 1;
        }

        free(key);
        free(value);
        i++;
    }
    info->exit_status = exit_status;
}
