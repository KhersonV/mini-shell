/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memset.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/05 11:30:52 by vmamoten          #+#    #+#             */
/*   Updated: 2024/03/11 10:25:28 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memset(void *ptr, int value, size_t num)
{
	unsigned char		*p;
	unsigned char		val;
	size_t				i;

	val = (unsigned char)value;
	p = (unsigned char *)ptr;
	i = 0;
	while (i < num)
	{
		*p++ = val;
		i++;
	}
	return (ptr);
}
