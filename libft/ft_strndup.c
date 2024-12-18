/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strndup.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/18 12:34:52 by vmamoten          #+#    #+#             */
/*   Updated: 2024/12/18 12:35:53 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strndup(const char *s, size_t n)
{
	char	*new_str;
	size_t	i;

	if (!s)
		return (NULL);
	new_str = (char *)malloc(n + 1); // Выделяем память с учетом '\0'
	if (!new_str)
		return (NULL);
	i = 0;
	while (i < n && s[i] != '\0') // Копируем до n символов или до конца строки
	{
		new_str[i] = s[i];
		i++;
	}
	new_str[i] = '\0'; // Добавляем завершающий нулевой символ
	return (new_str);
}
