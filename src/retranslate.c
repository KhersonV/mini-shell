/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   retranslate.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lynchsama <lynchsama@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/22 11:19:30 by vmamoten          #+#    #+#             */
/*   Updated: 2024/09/22 21:14:52 by lynchsama        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	ft_retranslate(t_tree *tree, char **argv)
{
	char	**args;

	args = argv;
	if (strcmp(tree->name, "echo") == 0)
		ft_echo(args);
	else if (strcmp(tree->name, "cd") == 0)
		ft_cd(args);
	else if (strcmp(tree->name, "pwd") == 0)
		ft_pwd();

}

/*
Предлагаю типизировать все билд ины
а конкретно
функция куда попадает дерево
ft_tree_analys(tree **tree)
{
	далее смотрим сколько там "Групп". другими словами сколько пайпов
	можно какие то доп переменные
	и просто передаем указатель на группу в билд ин
	например
	< test1.txt ls -l | grep test | sort > output.txt
	здесь мы смотрим, есть 3 группы
	и у нас получается здесь три указателя
	< test1.txt ls -l | grep test | sort > output.txt
	|					|			|
	первый				второй 		третий
	то есть
	tree *ptr1, *ptr2, ptr3
	и таким образом
	ft_ls(tree *ptr1)
	ft_gret(tree *ptr2)
	ft_sort(tree *ptr3)

	выходит что мы их типизировали и не нужно думать че и куда мы будем передавать
	это также поможет дальше, когда мы будем вызывать комманды через execve
	мы передадим туда этот же поинтер
	no_builtin_func(tree *ptr)
	потому что execve берет:  -имя комманды, -аргументы, -переменные окружения
	и это изи все может быть в струтуре


}

*/


//  < test1.txt ls -l | grep test | sort > output.txt