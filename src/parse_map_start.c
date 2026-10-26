/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_map_start.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esakgul <esakgul@student.42istanbul.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/20 15:00:00 by esakgul           #+#    #+#             */
/*   Updated: 2026/10/20 15:00:00 by esakgul          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

/*
** Walks the lines: blank lines are separators, map lines end the element
** section, everything else must be a valid element. Returns the index of
** the first map line, or -1 with the error recorded in p.
*/
int	find_map_start(t_parse *p, t_map *map)
{
	int	i;

	i = 0;
	while (p->lines[i])
	{
		if (is_map_line(p->lines[i]))
			return (i);
		if (!is_blank(p->lines[i]) && !parse_element(p, map, p->lines[i]))
			return (-1);
		i++;
	}
	set_err(p, "missing map", NULL);
	return (-1);
}

/* Exclusive end index: trailing blank lines after the map are dropped.   */
static int	map_end(char **lines, int start)
{
	int	end;

	end = start;
	while (lines[end])
		end++;
	while (end > start && is_blank(lines[end - 1]))
		end--;
	return (end);
}

/*
** Copies the raw map rows into map->grid. An element line inside or after
** the map is an error ("the map must be the last element"); blank lines
** inside the map are kept so phase 2 can reject them with context.
*/
int	collect_map(t_parse *p, t_map *map, int start)
{
	int	end;
	int	i;
	int	id;

	end = map_end(p->lines, start);
	map->grid = ft_calloc(end - start + 1, sizeof(char *));
	if (!map->grid)
		return (set_err(p, "memory allocation failed", NULL));
	i = start;
	while (i < end)
	{
		id = get_identifier(p->lines[i]);
		if (id >= 0)
			return (set_err(p, "element found after map start",
					ident_name(id)));
		if (!is_blank(p->lines[i]) && !is_map_line(p->lines[i]))
			return (set_err(p, "invalid line in map section", p->lines[i]));
		map->grid[i - start] = ft_strdup(p->lines[i]);
		if (!map->grid[i - start])
			return (set_err(p, "memory allocation failed", NULL));
		i++;
	}
	map->h = end - start;
	return (1);
}

int	check_all_set(t_parse *p, t_map *map)
{
	int	i;

	i = 0;
	while (i < 4)
	{
		if (!map->tex_path[i])
			return (set_err(p, "missing element", ident_name(i)));
		i++;
	}
	if (!map->floor_set)
		return (set_err(p, "missing element", ident_name(ID_F)));
	if (!map->ceil_set)
		return (set_err(p, "missing element", ident_name(ID_C)));
	if (!map->grid || !map->grid[0])
		return (set_err(p, "missing map", NULL));
	return (1);
}
