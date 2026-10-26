/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_file.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esakgul <esakgul@student.42istanbul.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/20 15:00:00 by esakgul           #+#    #+#             */
/*   Updated: 2026/10/20 15:00:00 by esakgul          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

/*
** Extension must be ".cub" with at least one character before it, and the
** file must be openable AND readable: open() succeeds on a directory, only
** read() reports EISDIR.
*/
static int	check_path(t_parse *p, const char *path)
{
	size_t	len;
	int		fd;
	char	c;

	len = ft_strlen(path);
	if (len < 5 || ft_strcmp(path + len - 4, ".cub") != 0)
		return (set_err(p, "scene file must have a .cub extension", path));
	fd = open(path, O_RDONLY);
	if (fd < 0)
		return (set_err(p, "cannot open scene file", path));
	if (read(fd, &c, 1) < 0)
	{
		close(fd);
		return (set_err(p, "cannot read scene file", path));
	}
	close(fd);
	return (1);
}

static int	count_lines(const char *path)
{
	int		fd;
	int		n;
	char	*line;

	fd = open(path, O_RDONLY);
	if (fd < 0)
		return (-1);
	n = 0;
	line = get_next_line(fd);
	while (line)
	{
		n++;
		free(line);
		line = get_next_line(fd);
	}
	close(fd);
	return (n);
}

/*
** Second pass: reads exactly p->n lines and strips the trailing newline.
** The extra get_next_line call drains gnl's static buffer (returns NULL).
*/
static int	read_lines(t_parse *p, const char *path)
{
	int		fd;
	int		i;
	char	*nl;

	p->lines = ft_calloc(p->n + 1, sizeof(char *));
	if (!p->lines)
		return (set_err(p, "memory allocation failed", NULL));
	fd = open(path, O_RDONLY);
	if (fd < 0)
		return (set_err(p, "cannot open scene file", path));
	i = 0;
	while (i < p->n)
	{
		p->lines[i] = get_next_line(fd);
		if (!p->lines[i])
			break ;
		nl = ft_strchr(p->lines[i], '\n');
		if (nl)
			*nl = '\0';
		i++;
	}
	free(get_next_line(fd));
	close(fd);
	return (1);
}

static int	file_is_blank(char **lines)
{
	int	i;

	i = 0;
	while (lines[i])
	{
		if (!is_blank(lines[i]))
			return (0);
		i++;
	}
	return (1);
}

int	load_file(t_parse *p, const char *path)
{
	if (!check_path(p, path))
		return (0);
	p->n = count_lines(path);
	if (p->n < 0)
		return (set_err(p, "cannot open scene file", path));
	if (!read_lines(p, path))
		return (0);
	if (file_is_blank(p->lines))
		return (set_err(p, "scene file is empty", path));
	return (1);
}
