/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: snazarov <snazarov@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/10 15:48:24 by vmamoten          #+#    #+#             */
/*   Updated: 2025/01/12 15:48:53 by snazarov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

void	main_initialize(t_info *info, char **envp)
{
	info->envp = NULL;
	info->input = NULL;
	info->std_in_reserve = dup(STDIN_FILENO);
	info->std_out_reserve = dup(STDOUT_FILENO);
	info->curr_dir = getcwd(NULL, 0);
	info->envp = copy_envp(envp);
	if (!info->envp)
		exit(EXIT_FAILURE);
	info->old_dir = get_env_value(info, "OLDPWD");
	info->home = get_env_value(info, "HOME");
	info->exit_status = 0;
	info->syntax_error = 0;
	init_env(info);
}

char	*get_user_input(void)
{
	char	*line;
	char	*temp;

	if (isatty(STDIN_FILENO))
		return (readline("minishell> "));
	line = get_next_line(STDIN_FILENO);
	if (line)
	{
		temp = line;
		line = ft_strtrim(line, "\n");
		free(temp);
	}
	return (line);
}

t_exec_command	*process_tokens(char *line, t_info *info, t_token **tokens)
{
	*tokens = tokenizer(line, info);
	if (!*tokens)
	{
		free(line);
		return (NULL);
	}
	adjusting_token_tree(tokens, info);
	if (info->syntax_error == 1)
	{
		free_token_list(*tokens);
		free(line);
		return (NULL);
	}
	return (parse_tokens_to_commands(*tokens));
}

void	process_input(char *line, t_info *info)
{
	t_token			*tokens;
	t_exec_command	*commands;

	tokens = NULL;
	commands = NULL;
	if (*line != '\0')
		add_history(line);
	info->input = line;
	commands = process_tokens(line, info, &tokens);
	if (!commands)
		return ;
	free_token_list(tokens);
	execute_commands(commands, info);
	free_array(commands);
	free(line);
}

int	main(int ac, char **av, char **envp)
{
	char	*line;
	t_info	info;

	if (ac != 1)
	{
		printf("minishell: %s: No such file or directory\n", av[1]);
		return (1);
	}
	main_initialize(&info, envp);
	
	while (1)
	{
		set_signal_mode_readline();
		line = get_user_input();
		if (line == NULL)
			exit_shell(&info);
		set_signal_mode_command();
		process_input(line, &info);
	}
	return (0);
}
