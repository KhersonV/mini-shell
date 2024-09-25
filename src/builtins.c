/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/12 17:08:50 by vmamoten          #+#    #+#             */
/*   Updated: 2024/09/25 14:13:58 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	ft_echo(char **args)
{
	int	newline;
	int	i;

	newline = 1;
	i = 1;
	if (args[1] && strcmp(args[1], "-n") == 0)
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

	if (!args[1] || strcmp(args[1], "~") == 0)
	{
		dir = getenv("HOME");
		if (!dir)
		{
			fprintf(stderr, "minishell: cd: HOME not set\n");
			return ;
		}
	}
	else if (strcmp(args[1], "-") == 0)
	{
		if (prev_dir[0] == '\0')
		{
			fprintf(stderr, "minishell: cd: OLDPWD not set\n");
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
			fprintf(stderr, "minishell: cd: HOME not set\n");
			return ;
		}
		snprintf(new_dir, sizeof(new_dir), "%s%s", home, args[1] + 1);
		dir = new_dir;
	}
	else
	{
		dir = args[1];
	}
	if (getcwd(cwd, sizeof(cwd)) == NULL)
	{
		perror("minishell: getcwd");
		return ;
	}
	strcpy(prev_dir, cwd);
	if (chdir(dir) != 0)
	{
		perror("minishell: cd");
		return ;
	}
	if (getcwd(cwd, sizeof(cwd)) != NULL)
	{
		setenv("OLDPWD", prev_dir, 1);
		setenv("PWD", cwd, 1);
	}
	else
	{
		perror("minishell: getcwd");
	}
}

void	ft_pwd(void)
{
	char	cwd[1024];

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

int	parse_and_set_env(char *arg)
{
	char	*key_value;
	char	*key;
	char	*value;

	key_value = strdup(arg);
	if (!key_value)
		return (0);
	key = strtok(key_value, "=");
	value = strtok(NULL, "=");
	if (key && value)
		setenv(key, value, 1);
	else
	{
		fprintf(stderr, "minishell: export: `%s': not a valid identifier\n",
			arg);
		free(key_value);
		return (0);
	}
	free(key_value);
	return (1);
}

void	ft_export(char **args, char ***env)
{
	if (!args[1])
	{
		print_env(*env);
	}
	else
	{
		if (!parse_and_set_env(args[1]))
			fprintf(stderr, "Error setting environment variable.\n");
	}
}

void	ft_unset(char **args, char ***envp)
{
	int	i;

	i = 1;
	if (!args[1])
	{
		fprintf(stderr, "minishell: unset: not enough arguments\n");
		return ;
	}
	while (args[i])
	{
		if (unsetenv(args[i]) != 0)
			perror("minishell: unset");
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
		exit_status = atoi(args[1]);
	exit(exit_status);
}
