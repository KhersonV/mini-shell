/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/03 12:04:48 by vmamoten          #+#    #+#             */
/*   Updated: 2024/12/30 12:43:55 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

size_t	ft__strlen(char const *s1)
{
	size_t	length;

	length = 0;
	if (s1 == NULL)
		return (0);
	while (s1[length] != '\0')
		length++;
	return (length);
}

char	*ft__strjoin(char *s1, char const *s2)
{
	char		*str;
	size_t		i;
	size_t		k;

	if (!s1 && !s2)
		return (NULL);
	str = (char *)malloc(ft__strlen(s1) + ft__strlen(s2) + 1);
	if (!str)
		return (NULL);
	k = 0;
	i = 0;
	while (s1 && s1[i] != '\0')
	{
		str[i] = s1[i];
		i++;
	}
	while (s2 && s2[k] != '\0')
	{
		str[i + k] = s2[k];
		k++;
	}
	str[i + k] = '\0';
	if (s1)
		free(s1);
	return (str);
}

char	*ft__strdup(const char *s1)
{
	unsigned long		i;
	unsigned long		length;
	char				*copied_string;

	if (!s1)
		return (NULL);
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

char	*ft__strchar(const char *s1, int c)
{
	if (!s1 || !*s1)
		return (NULL);
	while (*s1 != (char)c)
	{
		if (*s1 == '\0')
			return (NULL);
		s1++;
	}
	return ((char *)s1);
}

void	ft__strcpy(char *dest, char *src)
{
	int		i;

	i = 0;
	while (src[i] != '\0')
	{
		dest[i] = src[i];
		i++;
	}
	dest[i] = '\0';
}
