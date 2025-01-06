/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/10 14:25:29 by vmamoten          #+#    #+#             */
/*   Updated: 2025/01/06 17:59:11 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

void	export_no_args(t_info *info)
{
	char	**sorted_env;
	char	*equal_sign;
	int		i;

	sorted_env = copy_envp(info->envp);
	if (!sorted_env)
	{
		perror("minishell: export: failed to copy environment");
		info->exit_status = 1;
		return ;
	}
	sort_env(sorted_env);
	i = 0;
	while (sorted_env[i])
	{
		equal_sign = ft_strchr(sorted_env[i], '=');
		if (equal_sign)
			printf("declare -x %.*s=\"%s\"\n", (int)(equal_sign
					- sorted_env[i]), sorted_env[i], equal_sign + 1);
		else
			printf("declare -x %s\n", sorted_env[i]);
		i++;
	}
	ft_free_array(sorted_env);
	info->exit_status = 0;
}

int	export_invalid_option(char *arg, t_info *info)
{
	char	*str;

	str = "export: usage: export [-nf] [name[=value] ...] or export-p\n";
	if (arg[0] == '-' && arg[1] != '\0')
	{
		ft_putstr_fd("minishell: export: `", STDERR_FILENO);
		ft_putstr_fd(arg, STDERR_FILENO);
		ft_putstr_fd("': invalid option\n", STDERR_FILENO);
		ft_putstr_fd(str, STDERR_FILENO);
		info->exit_status = 2;
		return (1);
	}
	return (0);
}

void	export_parse_key_value(char *arg, char **key, char **value)
{
	char	*equal_sign;

	equal_sign = ft_strchr(arg, '=');
	if (equal_sign)
	{
		*key = ft_strndup(arg, equal_sign - arg);
		*value = ft_strdup(equal_sign + 1);
	}
	else
	{
		*key = ft_strdup(arg);
		*value = NULL;
	}
}

int	export_handle_key(t_info *info, char *arg)
{
	char	*key;
	char	*value;
	int		status;

	status = 0;
	export_parse_key_value(arg, &key, &value);
	if (!is_valid_env_key(key))
	{
		ft_putstr_fd("minishell: export: `", 2);
		ft_putstr_fd(arg, 2);
		ft_putstr_fd("': not a valid identifier\n", 2);
		status = 1;
	}
	else if (value)
		set_env(info, key, value);
	free(key);
	free(value);
	return (status);
}

void	ft_export(char **args, t_info *info)
{
	int	i;
	int	exit_status;

	if (!args[1])
	{
		export_no_args(info);
		return ;
	}
	exit_status = 0;
	i = 1;
	while (args[i])
	{
		if (export_invalid_option(args[i], info))
			return ;
		exit_status |= export_handle_key(info, args[i]);
		i++;
	}
	info->exit_status = exit_status;
}
