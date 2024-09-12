/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strtrim.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/11 09:48:06 by vmamoten          #+#    #+#             */
/*   Updated: 2024/03/11 09:48:06 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strtrim(const char *s1, const char *set)
{
	char	*trim;
	size_t	start;
	size_t	end;
	size_t	trim_len;

	if (!*s1)
		return (ft_strdup(""));
	if (!set[0])
		return (ft_strdup(s1));
	start = 0;
	end = ft_strlen(s1) - 1;
	while (s1[start] != '\0' && ft_strchr(set, s1[start]) != NULL)
		start++;
	while (end > start && ft_strchr(set, s1[end]) != NULL)
		end--;
	trim_len = end - start + 1;
	trim = (char *)malloc(trim_len + 1);
	if (!trim)
		return (NULL);
	ft_strlcpy(trim, &s1[start], trim_len + 1);
	return (trim);
}
