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

void	present_frame(t_game *game)
{
	mlx_put_image_to_window(game->mlx, game->win, game->frame.img, 0, 0);
}

/* One ray per screen column fills the whole frame, then one blit.        */
int	render_frame(t_game *game)
{
	cast_rays(game);
	present_frame(game);
	return (1);
}
