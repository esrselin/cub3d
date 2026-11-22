/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minimap_player_bonus.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ogokdas <ogokdas@student.42istanbul.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/22 17:50:43 by ogokdas           #+#    #+#             */
/*   Updated: 2026/11/22 17:50:44 by ogokdas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d_bonus.h"

/* One minimap pixel, given in panel coordinates, clipped to the panel.   */
static void	mm_dot(t_bgame *bg, int x, int y)
{
	if (x < 0 || y < 0 || x >= MM_SIZE || y >= MM_SIZE)
		return ;
	put_pixel(&bg->g.frame, MM_X + x, MM_Y + y, MM_PLAYER);
}

/*
** The player is drawn at its exact position (pos * cell, not rounded to
** a cell) as a small square, and its view direction as a line two cells
** long. dir is a unit vector, so dir * i walks i pixels along it.
*/
void	draw_minimap_player(t_bgame *bg)
{
	t_player	*pl;
	int			r;
	int			i;
	int			j;

	pl = &bg->g.player;
	r = bg->mm_cell / 4 + 1;
	j = -r;
	while (j <= r)
	{
		i = -r;
		while (i <= r)
			mm_dot(bg, pl->x * bg->mm_cell + i++, pl->y * bg->mm_cell + j);
		j++;
	}
	i = 0;
	while (i < 2 * bg->mm_cell)
	{
		mm_dot(bg, pl->x * bg->mm_cell + pl->dir_x * i,
			pl->y * bg->mm_cell + pl->dir_y * i);
		i++;
	}
}
