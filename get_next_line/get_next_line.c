/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/11 13:21:09 by vmamoten          #+#    #+#             */
/*   Updated: 2024/12/30 12:43:05 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

char	*ft_check_read(int fd, char *buffer, char *res)
{
	ssize_t	bytes_read;

	bytes_read = 1;
	while (bytes_read > 0 && !ft__strchar(res, '\n'))
	{
		bytes_read = read(fd, buffer, BUFFER_SIZE);
		if (bytes_read < 0)
		{
			if (res)
				free(res);
			return (buffer[0] = '\0', NULL);
		}
		if (bytes_read > 0)
		{
			buffer[bytes_read] = '\0';
			res = ft__strjoin(res, buffer);
			if (ft__strchar(res, '\n'))
				return (res);
		}
	}
	return (res);
}

char	*ft_split_line(char *res, char *buffer)
{
	char	*splitted_line;
	int		i;

	if (res == NULL)
		return (NULL);
	splitted_line = ft__strdup(res);
	if (splitted_line == NULL)
		return (NULL);
	ft__strcpy(splitted_line, res);
	i = 1;
	while (splitted_line[i - 1] != '\n' && splitted_line[i - 1] != '\0'
		&& splitted_line[i] != '\0')
		i++;
	splitted_line[i] = '\0';
	ft__strcpy(buffer, res + i);
	free(res);
	return (splitted_line);
}

char	*get_next_line(int fd)
{
	static char	buffer[BUFFER_SIZE + 1];
	char		*res;

	res = NULL;
	if (buffer[0])
		res = ft__strjoin(res, buffer);
	if (ft__strchar(res, '\n'))
		return (ft_split_line(res, buffer));
	res = ft_check_read(fd, buffer, res);
	if (!res)
		return (NULL);
	return (ft_split_line(res, buffer));
}
