/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lynchsama <lynchsama@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/22 21:23:27 by lynchsama         #+#    #+#             */
/*   Updated: 2024/09/22 21:25:57 by lynchsama        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

typedef struct s_token
{
	char	*str;
	int		type;
	struct s_token *next;
	struct s_token *prev;
}	t_token;




int main(int argc, char **argv)
{

}