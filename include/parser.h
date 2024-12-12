/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/12 12:53:20 by vmamoten          #+#    #+#             */
/*   Updated: 2024/12/12 13:52:05 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_H
# define PARSER_H

# include "minishell.h"

typedef struct s_node
{
	char			*data;
	struct s_node	*left;
	struct s_node	*right;
	char			*args;
	char			*redirect_op;
	char			*redirect_file;
}					t_node;

t_node				*parse_tokens(t_token *tokens);
void				free_ast(t_node *node);
void				print_ast(t_node *node);

#endif
