/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_element.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esakgul <esakgul@student.42istanbul.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/20 15:00:00 by esakgul           #+#    #+#             */
/*   Updated: 2026/10/20 15:00:00 by esakgul          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

static void	tabs_to_spaces(char *s)
{
	while (*s)
	{
		if (*s == '\t')
			*s = ' ';
		s++;
	}
}

/* open() alone is not enough: a directory opens fine but read() fails.   */
static int	check_readable(t_parse *p, const char *path)
{
	int		fd;
	ssize_t	n;
	char	c;

	fd = open(path, O_RDONLY);
	if (fd < 0)
		return (set_err(p, "cannot open texture", path));
	n = read(fd, &c, 1);
	close(fd);
	if (n < 0)
		return (set_err(p, "cannot read texture", path));
	if (n == 0)
		return (set_err(p, "texture file is empty", path));
	return (1);
}

static int	set_texture(t_parse *p, t_map *map, int side, const char *path)
{
	size_t	len;

	if (map->tex_path[side])
		return (set_err(p, "duplicate identifier", ident_name(side)));
	len = ft_strlen(path);
	if (len < 5 || ft_strcmp(path + len - 4, ".xpm") != 0)
		return (set_err(p, "texture must be a .xpm file", path));
	if (!check_readable(p, path))
		return (0);
	map->tex_path[side] = ft_strdup(path);
	if (!map->tex_path[side])
		return (set_err(p, "memory allocation failed", NULL));
	return (1);
}

static int	set_color(t_parse *p, t_map *map, int which, const char *tok)
{
	int	*set;
	int	*rgb;

	set = &map->floor_set;
	rgb = &map->floor_rgb;
	if (which == ID_C)
	{
		set = &map->ceil_set;
		rgb = &map->ceil_rgb;
	}
	if (*set)
		return (set_err(p, "duplicate identifier", ident_name(which)));
	if (!parse_rgb(p, tok, rgb))
		return (0);
	*set = 1;
	return (1);
}

/*
** Parses one non-blank line that is not a map line. Tabs become spaces so
** ft_split sees a single separator; exactly two tokens are required.
*/
int	parse_element(t_parse *p, t_map *map, char *line)
{
	char	**tok;
	int		id;
	int		ok;

	tabs_to_spaces(line);
	tok = ft_split(line, ' ');
	if (!tok || !tok[0])
	{
		ft_free_tab(tok);
		return (set_err(p, "invalid line", line));
	}
	id = get_identifier(tok[0]);
	if (id < 0)
		ok = set_err(p, "unknown identifier", tok[0]);
	else if (!tok[1])
		ok = set_err(p, "missing value for identifier", tok[0]);
	else if (tok[2])
		ok = set_err(p, "too many values for identifier", tok[0]);
	else if (id < ID_F)
		ok = set_texture(p, map, id, tok[1]);
	else
		ok = set_color(p, map, id, tok[1]);
	ft_free_tab(tok);
	return (ok);
}
