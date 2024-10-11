/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: admin <admin@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/12 17:08:50 by vmamoten          #+#    #+#             */
/*   Updated: 2024/10/11 23:49:44 by admin            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	ft_echo(char **args)
{
	int	i;
	int	newline;

	i = 1;
	newline = 1;
	while (args[i] && ft_strcmp(args[i], "-n") == 0)
	{
		newline = 0;
		i++;
	}
	while (args[i])
	{
		printf("%s", args[i]);
		if (args[i + 1])
			printf(" ");
		i++;
	}
	if (newline)
		printf("\n");
}

void	ft_cd(char **args)
{
	char		*dir;
	char		cwd[PATH_MAX];
	static char	prev_dir[PATH_MAX] = "";
	char		*home;
	char		new_dir[PATH_MAX];

	if (!args[1] || ft_strcmp(args[1], "~") == 0)
	{
		home = getenv("HOME");
		if (!home)
		{
			printf("minishell: cd: HOME not set\n");
			return ;
		}
		dir = home;
	}
	else if (ft_strcmp(args[1], "-") == 0)
	{
		if (prev_dir[0] == '\0')
		{
			printf("minishell: cd: OLDPWD not set\n");
			return ;
		}
		dir = prev_dir;
		printf("%s\n", dir);
	}
	else if (args[1][0] == '~')
	{
		home = getenv("HOME");
		if (!home)
		{
			printf("minishell: cd: HOME not set\n");
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
		return ;
	}
	ft_strlcpy(prev_dir, cwd, PATH_MAX);
	if (chdir(dir) != 0)
	{
		perror("minishell: cd");
		return ;
	}
}

void	ft_pwd(void)
{
	char	cwd[PATH_MAX];

	if (getcwd(cwd, sizeof(cwd)) != NULL)
		printf("%s\n", cwd);
	else
		perror("minishell: pwd");
}

void	print_env(char **env)
{
	int	i;

	i = 0;
	while (env[i])
	{
		printf("declare -x %s\n", env[i]);
		i++;
	}
}

void    ft_add_env_var(char ***envp, char *key, char *value)
{
	(void)envp;
	(void)key;
	(void)value;
}

void	ft_export(char **args, char ***env)
{
	int		i;
	char	*key;
	char	*value;
	// char	*new_var;
	
	if (!args[1])
	{
		print_env(*env);
		return;
	}
	
	i = 1;
	while (args[i])
	{
		key = args[i];
		value = ft_strchr(key, '=');
		if (value)
		{
			*value = '\0';
			value++;
			ft_add_env_var(env, key, value);
		}
		else
            printf("minishell: export: `%s': not a valid identifier\n", args[i]);
        i++;
	}
	
}

void    ft_remove_env_var(char ***envp, char *key)
{
	(void)envp;
	(void)key;
}

void	ft_unset(char **args, char ***envp)
{
	int	i;

	i = 1;
	if (!args[1])
	{
		printf("minishell: unset: not enough arguments\n");
		return ;
	}
	while (args[i])
	{
		ft_remove_env_var(envp, args[i]);
		i++;
	}
}

void	ft_env(char **env)
{
	int	i;

	i = 0;
	while (env[i])
	{
		printf("%s\n", env[i]);
		i++;
	}
}

void	ft_exit(char **args)
{
	int	exit_status;

	exit_status = 0;
	if (args[1])
		exit_status = ft_atoi(args[1]);
	exit(exit_status);
}
