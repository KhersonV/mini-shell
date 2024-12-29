/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/10 14:25:29 by vmamoten          #+#    #+#             */
/*   Updated: 2024/12/29 14:55:57 by vmamoten         ###   ########.fr       */
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
void ft_export(char **args, t_info *info)
{
    char **sorted_env;
    int i;
    char *key;
    char *value;

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
            char *equal_sign = ft_strchr(sorted_env[i], '=');
            if (equal_sign)
            {
                printf("declare -x %.*s=\"%s\"\n",
                       (int)(equal_sign - sorted_env[i]),
                       sorted_env[i],
                       equal_sign + 1);
            }
            else
            {
                printf("declare -x %s\n", sorted_env[i]);
            }
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
        char *equal_sign = ft_strchr(args[i], '=');
        if (equal_sign) // Формат key=value
        {
            key = ft_strndup(args[i], equal_sign - args[i]);
            value = ft_strdup(equal_sign + 1);
        }
        else // Формат только key
        {
            key = ft_strdup(args[i]);
            value = NULL;
        }

        if (is_valid_env_key(key)) // Проверка ключа
        {
            if (value)
                set_env(info, key, value); // Обновить или добавить key=value
            info->exit_status = 0;
        }
        else
        {
            ft_putstr_fd("minishell: export: `", 2);
            ft_putstr_fd(args[i], 2);
            ft_putstr_fd("': not a valid identifier\n", 2);
            info->exit_status = 1;
        }

        free(key);
        free(value);
        i++;
    }
}
