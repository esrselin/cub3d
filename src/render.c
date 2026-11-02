/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esakgul <esakgul@student.42istanbul.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/20 15:00:00 by esakgul           #+#    #+#             */
/*   Updated: 2026/10/20 15:00:00 by esakgul          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

/*
** Vertical sampling. The wall slice is line_h pixels tall but only the
** part inside the screen is drawn, so tex_pos starts at the texture row
** that corresponds to draw_start: (draw_start - (H/2 - line_h/2)) * step,
** where step = tex_h / line_h texture rows per screen pixel. tex_y is
** clamped rather than masked, so textures need not be a power of two.
*/
void	draw_wall(t_game *game, t_ray *ray, int x)
{
	t_tex	*tex;
	double	step;
	double	tex_pos;
	int		y;
	int		tex_y;

	tex = &game->tex[ray->face];
	tex_column(game, ray);
	step = (double)tex->h / ray->line_h;
	tex_pos = (ray->draw_start - WIN_H / 2 + ray->line_h / 2) * step;
	y = ray->draw_start;
	while (y <= ray->draw_end)
	{
		tex_y = (int)tex_pos;
		if (tex_y >= tex->h)
			tex_y = tex->h - 1;
		put_pixel(&game->frame, x, y, get_texel(tex, ray->tex_x, tex_y));
		tex_pos += step;
		y++;
	}
}

/* Ceiling above draw_start, textured wall between, floor below draw_end. */
void	draw_column(t_game *game, t_ray *ray, int x)
{
	int	y;

	y = 0;
	while (y < ray->draw_start)
		put_pixel(&game->frame, x, y++, game->map.ceil_rgb);
	draw_wall(game, ray, x);
	y = ray->draw_end + 1;
	while (y < WIN_H)
		put_pixel(&game->frame, x, y++, game->map.floor_rgb);
}

/*
** One ray per column. Reading from four texture buffers per frame is
** plain memory access (~800k texel reads at 1024x768), far cheaper than
** mlx_pixel_put, which issues an X request per pixel.
*/
void	cast_rays(t_game *game)
{
	t_ray	ray;
	int		x;

	x = 0;
	while (x < WIN_W)
	{
		init_ray(game, &ray, x);
		init_step(game, &ray);
		perform_dda(game, &ray);
		compute_wall(&ray);
		draw_column(game, &ray, x);
		x++;
	}
}
