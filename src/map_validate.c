/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   map_validate.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esakgul <esakgul@student.42istanbul.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/20 15:00:00 by esakgul           #+#    #+#             */
/*   Updated: 2026/10/20 15:00:00 by esakgul          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

static int	map_width(char **grid)
{
	int	w;
	int	len;
	int	i;

	w = 0;
	i = 0;
	while (grid[i])
	{
		len = ft_strlen(grid[i]);
		if (len > w)
			w = len;
		i++;
	}
	return (w);
}

/*
** Pads every row with spaces to the same width, so grid[y][x] is valid for
** 0 <= x < w and 0 <= y < h. Costs one allocation per row; saves a strlen
** on every neighbour lookup here and on every DDA step in the renderer.
*/
static int	pad_grid(t_parse *p, t_map *map)
{
	char	*row;
	int		y;

	map->w = map_width(map->grid);
	y = 0;
	while (y < map->h)
	{
		row = malloc(map->w + 1);
		if (!row)
			return (set_err(p, "memory allocation failed", NULL));
		ft_memset(row, ' ', map->w);
		row[map->w] = '\0';
		ft_memcpy(row, map->grid[y], ft_strlen(map->grid[y]));
		free(map->grid[y]);
		map->grid[y] = row;
		y++;
	}
	return (1);
}

/* Anything outside the rectangle is treated as a space (= void).         */
char	map_cell(t_map *map, int x, int y)
{
	if (x < 0 || y < 0 || x >= map->w || y >= map->h)
		return (' ');
	return (map->grid[y][x]);
}

/*
** A walkable cell ('0' or the player) must be enclosed: none of its 8
** neighbours may be a space or lie outside the map. 8-connectivity is
** stricter than 4: it also rejects a diagonal gap between two walls.
*/
static int	check_enclosed(t_parse *p, t_map *map, int x, int y)
{
	int	dx;
	int	dy;

	dy = -1;
	while (dy <= 1)
	{
		dx = -1;
		while (dx <= 1)
		{
			if (map_cell(map, x + dx, y + dy) == ' ')
				return (set_err(p, "map is not closed, see row",
						map->grid[y]));
			dx++;
		}
		dy++;
	}
	return (1);
}

/*
** Iterative neighbour check over every cell: O(w*h), no recursion, no
** extra memory, and it also catches holes in regions the player cannot
** reach. Blank lines inside the map and foreign characters are rejected.
*/
int	validate_map(t_parse *p, t_game *game)
{
	t_map	*map;
	int		x;
	int		y;

	map = &game->map;
	if (!pad_grid(p, map))
		return (0);
	y = 0;
	while (y < map->h)
	{
		if (is_blank(map->grid[y]))
			return (set_err(p, "empty line inside map", NULL));
		x = 0;
		while (x < map->w)
		{
			if (!ft_strchr("01NSEW ", map->grid[y][x]))
				return (set_err(p, "invalid character in map", map->grid[y]));
			if (ft_strchr("0NSEW", map->grid[y][x])
				&& !check_enclosed(p, map, x, y))
				return (0);
			x++;
		}
		y++;
	}
	return (find_player(p, game));
}
