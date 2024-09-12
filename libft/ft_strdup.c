/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strdup.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/10 11:07:42 by vmamoten          #+#    #+#             */
/*   Updated: 2024/03/10 13:44:51 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strdup(const char *s1)
{
	int		i;
	int		length;
	char	*copied_string;

	i = 0;
	length = 0;
	while (s1[length])
		length++;
	copied_string = malloc(length + 1);
	if (!copied_string)
		return (NULL);
	while (s1[i])
	{
		copied_string[i] = s1[i];
		i++;
	}
	copied_string[i] = '\0';
	return (copied_string);
}
