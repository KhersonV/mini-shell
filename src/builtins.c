/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: admin <admin@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/12 17:08:50 by vmamoten          #+#    #+#             */
/*   Updated: 2024/10/15 13:55:30 by admin            ###   ########.fr       */
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

int find_envp_var(char **envp, char *var)
{
	int	leng;
	int i;
	
	i = 0;
	leng = ft_strlen(var);
	while (envp[i])
	{
		if (ft_strncmp(envp[i], var, leng) == 0 && envp[i][leng] == '=')
		{
			return (i);
		}
		i++;
	}
	return (-1);
}

int ft_add_new(char ***envp, char *new_var) 
{
	int env_len;
	char **new_envp;
	int		i;

	i = 0;
	env_len = 0;
	while ((*envp)[env_len])
	{
		env_len++;
	}
	new_envp = malloc(sizeof(char *) * (env_len + 2));
	if (!new_envp)
	{
		free(new_var);
		return (-1);
	}
	while (i < env_len)
	{
		new_envp[i] = (*envp)[i];
		i++; 
	}
	new_envp[env_len] = new_var;
	new_envp[env_len + 1] = NULL;
	free(*envp);
	*envp = new_envp;
	return (0);
}

int		set_env_var(char ***envp, char *var, char *value)
{
	int	i;
	char	*new_var;
	char	*temp;
	
	i = find_envp_var(*envp, var);
	temp = ft_strjoin(var, "=");
	if (!temp)
		return (-1);
	new_var = ft_strjoin(temp, value);
	free(temp);
	if (!new_var)
		return (-1);
	if (i >= 0)
	{
		free((*envp)[i]);
		(*envp)[i] = new_var;
	}
	else
	{
		if (ft_add_new(envp, new_var) == -1)
			return (-1);
	}
	return (0);
}

char	*get_env_value(char **envp, char *var)
{
	int i;
	int len;

	len = ft_strlen(var);
	i = 0;
	while (envp[i])
	{
		if (ft_strncmp(envp[i], var, len) == 0 && envp[i][len] == '=')
			return (envp[i] + len + 1);
		i++;
	}
	return (NULL);
}


void	ft_cd(char **args, char ***envp)
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
	if (chdir(dir) != 0)
	{
		perror("minishell: cd");
		return ;
	}
	pwd_value = get_env_value(*envp, "PWD");
	if (pwd_value != NULL)
	{
		if (set_env_var(envp, "OLDPWD", pwd_value) == -1)
		{
			printf("minishell: cd: failed to set OLDPWD\n");
			return ;
		}
	}
	else
	{
		if (set_env_var(envp, "OLDPWD", cwd) == -1)
		{
			printf("minishell: cd: failed to set OLDPWD\n");
			return ;
		}
	}
	if (getcwd(cwd, sizeof(cwd)) == NULL)
	{
		perror("minishell: getcwd");
		return ;
	}
	if (set_env_var(envp, "PWD", cwd) == -1)
	{
		printf("minishell: cd: failed to set PWD\n");
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

void	ft_add_env_var(char ***envp, char *key, char *value)
{
	set_env_var(envp, key, value);
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
		return ;
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
			printf("minishell: export: `%s': not a valid identifier\n",
				args[i]);
		i++;
	}
}

void	ft_remove_env_var(char ***envp, char *key)
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
