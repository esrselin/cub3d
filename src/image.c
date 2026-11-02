/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   image.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esakgul <esakgul@student.42istanbul.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/20 15:00:00 by esakgul           #+#    #+#             */
/*   Updated: 2026/10/20 15:00:00 by esakgul          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

/*
** addr is a byte buffer: row y starts at y * line_len (bytes, may include
** padding beyond w * 4), pixel x is x * (bpp / 8) bytes further. With
** bpp == 32 the pixel is written as one unsigned int in 0x00RRGGBB form,
** which matches the little-endian BGRA layout mlx uses on X11.
*/
void	put_pixel(t_tex *img, int x, int y, int color)
{
	char	*dst;

	if (x < 0 || y < 0 || x >= img->w || y >= img->h)
		return ;
	dst = img->addr + y * img->line_len + x * (img->bpp / 8);
	*(unsigned int *)dst = (unsigned int)color;
}

/* Ceiling colour on the upper half, floor colour on the lower half.      */
void	draw_background(t_game *game)
{
	int	x;
	int	y;
	int	color;

	y = 0;
	while (y < WIN_H)
	{
		color = game->map.ceil_rgb;
		if (y >= WIN_H / 2)
			color = game->map.floor_rgb;
		x = 0;
		while (x < WIN_W)
		{
			put_pixel(&game->frame, x, y, color);
			x++;
		}
		y++;
	}
}

void	present_frame(t_game *game)
{
	mlx_put_image_to_window(game->mlx, game->win, game->frame.img, 0, 0);
}

/* Full redraw: background now, walls from phase 4 on top of it.          */
int	render_frame(t_game *game)
{
	draw_background(game);
	present_frame(game);
	return (1);
}
