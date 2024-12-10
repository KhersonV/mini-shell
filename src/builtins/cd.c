/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/10 14:25:04 by vmamoten          #+#    #+#             */
/*   Updated: 2024/12/10 14:59:39 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/builtins.h"

void	ft_cd(char **args, char ***envp, t_info *info)
{
	char	*dir;
	char	cwd[PATH_MAX];
	char	*old_pwd;
	char	*home;
	char	new_dir[PATH_MAX];
	char	*pwd_value;

	if (!args[1] || ft_strcmp(args[1], "~") == 0)
	{
		home = get_env_value(*envp, "HOME");
		if (!home)
		{
			printf("minishell: cd: HOME not set\n");
			info->exit_status = 1;
			return ;
		}
		dir = home;
	}
	else if (ft_strcmp(args[1], "-") == 0)
	{
		old_pwd = get_env_value(*envp, "OLDPWD");
		if (!old_pwd)
		{
			printf("minishell: cd: OLDPWD not set\n");
			info->exit_status = 1;
			return ;
		}
		dir = old_pwd;
		printf("%s\n", dir);
	}
	else if (args[1][0] == '~')
	{
		home = get_env_value(*envp, "HOME");
		if (!home)
		{
			printf("minishell: cd: HOME not set\n");
			info->exit_status = 1;
			return ;
		}
		ft_strlcpy(new_dir, home, PATH_MAX);
		ft_strlcat(new_dir, args[1] + 1, PATH_MAX);
		dir = new_dir;
	}
	else
		dir = args[1];
	if (getcwd(cwd, sizeof(cwd)) == NULL)
	{
		perror("minishell: getcwd");
		info->exit_status = 1;
		return ;
	}
	if (chdir(dir) != 0)
	{
		perror("minishell: cd");
		info->exit_status = 1;
		return ;
	}
	pwd_value = get_env_value(*envp, "PWD");
	if (pwd_value != NULL)
	{
		if (set_env_var(envp, "OLDPWD", pwd_value) == -1)
		{
			printf("minishell: cd: failed to set OLDPWD\n");
			info->exit_status = 1;
			return ;
		}
	}
	else
	{
		if (set_env_var(envp, "OLDPWD", cwd) == -1)
		{
			printf("minishell: cd: failed to set OLDPWD\n");
			info->exit_status = 1;
			return ;
		}
	}
	if (getcwd(cwd, sizeof(cwd)) == NULL)
	{
		perror("minishell: getcwd");
		info->exit_status = 1;
		return ;
	}
	if (set_env_var(envp, "PWD", cwd) == -1)
	{
		printf("minishell: cd: failed to set PWD\n");
		info->exit_status = 1;
		return ;
	}
	info->exit_status = 0;
}

