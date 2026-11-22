/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minimap_bonus.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ogokdas <ogokdas@student.42istanbul.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/22 17:39:55 by ogokdas           #+#    #+#             */
/*   Updated: 2026/11/22 17:39:56 by ogokdas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d_bonus.h"

/*
** Pixels per map cell: the whole map fits in MM_SIZE when it can, but a
** cell is never larger than MM_CELL_MAX nor smaller than MM_CELL_MIN.
*/
static int	mm_cell_size(t_map *map)
{
	int	cell;

	cell = MM_CELL_MAX;
	if (MM_SIZE / map->w < cell)
		cell = MM_SIZE / map->w;
	if (MM_SIZE / map->h < cell)
		cell = MM_SIZE / map->h;
	if (cell < MM_CELL_MIN)
		cell = MM_CELL_MIN;
	return (cell);
}

/* Dark background behind the cells, MM_PAD wider on every side.          */
static void	mm_panel(t_bgame *bg)
{
	int	w;
	int	h;
	int	x;
	int	y;

	w = bg->g.map.w * bg->mm_cell;
	h = bg->g.map.h * bg->mm_cell;
	if (w > MM_SIZE)
		w = MM_SIZE;
	if (h > MM_SIZE)
		h = MM_SIZE;
	y = -MM_PAD;
	while (y < h + MM_PAD)
	{
		x = -MM_PAD;
		while (x < w + MM_PAD)
			put_pixel(&bg->g.frame, MM_X + x++, MM_Y + y, MM_BG);
		y++;
	}
}

/*
** One map cell, one pixel smaller than its slot so the panel shows
** through as grid lines. Pixels past MM_SIZE are clipped.
*/
static void	mm_fill(t_bgame *bg, int cx, int cy, int color)
{
	int	x;
	int	y;

	y = cy * bg->mm_cell;
	while (y < (cy + 1) * bg->mm_cell - 1 && y < MM_SIZE)
	{
		x = cx * bg->mm_cell;
		while (x < (cx + 1) * bg->mm_cell - 1 && x < MM_SIZE)
		{
			put_pixel(&bg->g.frame, MM_X + x, MM_Y + y, color);
			x++;
		}
		y++;
	}
}

/*
** Drawn into the frame buffer after the 3D view, before the single
** mlx_put_image_to_window. Spaces (outside the map) stay panel coloured.
*/
void	draw_minimap(t_bgame *bg)
{
	t_map	*map;
	int		cx;
	int		cy;

	map = &bg->g.map;
	bg->mm_cell = mm_cell_size(map);
	mm_panel(bg);
	cy = 0;
	while (cy < map->h && cy * bg->mm_cell < MM_SIZE)
	{
		cx = 0;
		while (cx < map->w && cx * bg->mm_cell < MM_SIZE)
		{
			if (map->grid[cy][cx] == '1')
				mm_fill(bg, cx, cy, MM_WALL);
			else if (map->grid[cy][cx] == '0')
				mm_fill(bg, cx, cy, MM_FLOOR);
			cx++;
		}
		cy++;
	}
}
