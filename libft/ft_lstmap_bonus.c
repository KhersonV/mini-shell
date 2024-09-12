/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstmap_bonus.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/15 12:34:09 by vmamoten          #+#    #+#             */
/*   Updated: 2024/03/15 19:42:39 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

t_list	*ft_lstmap(t_list *lst, void *(*f)(void *), void (*del)(void *))
{
	t_list		*modified_list;
	t_list		*new_node;
	void		*result;

	modified_list = NULL;
	while (lst)
	{
		if (f)
			result = f(lst->content);
		if (!result)
		{
			ft_lstclear(&modified_list, del);
			return (NULL);
		}
		new_node = ft_lstnew(result);
		if (!new_node)
		{
			ft_lstclear(&modified_list, del);
			del(result);
			return (NULL);
		}
		ft_lstadd_back(&modified_list, new_node);
		lst = lst->next;
	}
	return (modified_list);
}
