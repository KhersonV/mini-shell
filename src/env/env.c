/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/11 12:43:01 by vmamoten          #+#    #+#             */
/*   Updated: 2025/01/02 13:34:27 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

char	**copy_envp(char **envp)
{
	int		i;
	int		j;
	char	**env_copy;

	if (!envp)
		return (NULL);
	i = 0;
	while (envp[i])
		i++;
	env_copy = malloc(sizeof(char *) * (i + 1));
	if (!env_copy)
		return (NULL);
	i = 0;
	while (envp[i])
	{
		env_copy[i] = ft_strdup(envp[i]);
		if (!env_copy[i])
		{
			j = 0;
			while (j < i)
			{
				free(env_copy[j]);
				j++;
			}
			free(env_copy);
			return (NULL);
		}
		i++;
	}
	env_copy[i] = NULL;
	return (env_copy);
}

#include "../../include/minishell.h"

void init_env(t_info *info, char **envp)
{
    char *shlvl_value;
    int shlvl;
    char new_shlvl[12];
    int i;
    char *err = "minishell: warning: shell level too high, resetting to 1\n";

    // Копируем окружение
    info->envp = copy_envp(envp);
    if (!info->envp)
        exit(EXIT_FAILURE);

    // Получаем текущее значение SHLVL
    shlvl_value = get_env_value(info, "SHLVL");
    if (shlvl_value)
    {
        shlvl = ft_atoi(shlvl_value);
        free(shlvl_value);

        // Увеличиваем SHLVL
        shlvl++;
        if (shlvl > 999) // В оригинальном bash, SHLVL обнуляется, если > 999
        {
            ft_putstr_fd(err, STDERR_FILENO);
            shlvl = 1;
        }
    }
    else
    {
        // Если SHLVL не существует, устанавливаем его в 1
        shlvl = 1;
    }

    // Конвертируем число SHLVL в строку
    i = 0;
    while (shlvl > 0)
    {
        new_shlvl[i++] = (shlvl % 10) + '0';
        shlvl /= 10;
    }
    new_shlvl[i] = '\0';

    // Переворачиваем строку с числом
    int j = 0;
    while (j < i / 2)
    {
        char tmp = new_shlvl[j];
        new_shlvl[j] = new_shlvl[i - j - 1];
        new_shlvl[i - j - 1] = tmp;
        j++;
    }

    // Обновляем или добавляем SHLVL в окружение
    if (set_env(info, "SHLVL", new_shlvl) == -1)
        exit(EXIT_FAILURE);
}


char *get_env_value(t_info *info, const char *key)
{
    int i = 0;
    char *value;

    while (info->envp[i])
    {
        if (env_key_compare(info->envp[i], key))
        {
            value = get_value_from_env(info->envp[i]);
            if (value && value[0] == '\0')
            {
                free(value);
                return (NULL);
            }
            return (value);
        }
        i++;
    }
    return (NULL);
}


int set_env(t_info *info, const char *key, const char *value)
{
    int     i;
    char    *new_entry;

    if (!info || !key || !value || !is_valid_env_key(key))
    {
        fprintf(stderr, "minishell: export: `%s': not a valid identifier\n", key);
        info->exit_status = 1;
        return (-1);
    }
    new_entry = create_env_entry(key, value);
    if (!new_entry)
    {
        perror("minishell: export: failed to create env entry");
        info->exit_status = 1;
        return (-1);
    }
    i = 0;
    while (info->envp[i])
    {
        if (env_key_compare(info->envp[i], key))
        {
            free(info->envp[i]);
            info->envp[i] = new_entry;
            info->exit_status = 0;

            if (strcmp(key, "HOME") == 0)
            {
                free(info->home);
                info->home = ft_strdup(value);
                if (!info->home)
                    return (-1);
            }

            return (0);
        }
        i++;
    }
    info->envp = append_env_entry(info->envp, new_entry);

    if (strcmp(key, "HOME") == 0)
    {
        free(info->home);
        info->home = ft_strdup(value);
        if (!info->home)
            return (-1);
    }

    info->exit_status = 0;
    return (0);
}

char	**env_to_array(t_info *info)
{
	return (copy_envp(info->envp));
}

void	free_env(t_info *info)
{
	int	i;

	if (!info || !info->envp)
		return ;
	i = 0;
	while (info->envp[i])
	{
		free(info->envp[i]);
		i++;
	}
	free(info->envp);
	info->envp = NULL;
}
