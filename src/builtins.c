/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: admin <admin@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/12 17:08:50 by vmamoten          #+#    #+#             */
/*   Updated: 2024/09/13 13:22:49 by admin            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	ft_strcmp(const char *s1, const char *s2)
{
	while (*s1 && (*s1 == *s2))
	{
		s1++;
		s2++;
	}
	return ((unsigned char)*s1 - (unsigned char)*s2);
}

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
	if (!args[1] || chdir(args[1]) != 0)
		perror("minishell: cd");
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

void	ft_unset(char **args, char ***env)
{
	if (args[1])
	{
		if (unsetenv(args[1]) != 0)
			perror("minishell: unset");
	}
	else
		fprintf(stderr, "minishell: unset: not enough arguments\n");
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