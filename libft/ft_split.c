/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/12 11:02:50 by vmamoten          #+#    #+#             */
/*   Updated: 2024/03/14 13:30:47 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include <stdio.h>

static size_t	ft_count_word(const char *str, char c)
{
	size_t	i;
	size_t	count;

	i = 0;
	count = 0;
	while (str[i] != '\0')
	{
		if (str[i] != c && (i == 0 || str[i - 1] == c))
			count++;
		i++;
	}
	return (count);
}

static size_t	ft_get_word(const char *s, char c, size_t indx)
{
	size_t	i;

	i = 0;
	if (c == '\0')
	{
		while (s[indx + i] != '\0')
			i++;
	}
	else
	{
		while (s[indx + i] != c && s[indx + i] != '\0')
			i++;
	}
	return (i);
}

static void	ft_fill_str(const char *s, char **str, char c)
{
	size_t	i;
	size_t	k;
	size_t	leng_w;

	i = 0;
	k = 0;
	while (s[i])
	{
		if (s[i] != c && (i == 0 || s[i - 1] == c))
		{
			leng_w = ft_get_word(s, c, i);
			str[k] = ft_substr(s, i, leng_w);
			if (!str[k])
			{
				while (k > 0)
					free(str[--k]);
				str[0] = NULL;
				return ;
			}
			k++;
		}
		i++;
	}
}

char	**ft_split(char const *s, char c)
{
	size_t		num;
	char		**str;

	if (!s)
		return (NULL);
	num = ft_count_word(s, c);
	if (num == 0)
	{
		str = (char **)malloc((num + 1) * sizeof(char *));
		if (!str)
			return (NULL);
		str[num] = NULL;
		return (str);
	}
	str = (char **)malloc((num + 1) * sizeof(char *));
	if (!str)
		return (NULL);
	ft_fill_str(s, str, c);
	if (str[0] == NULL)
	{
		free(str);
		return (0);
	}
	str[num] = NULL;
	return (str);
}
