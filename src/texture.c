/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   texture.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esakgul <esakgul@student.42istanbul.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/20 15:00:00 by esakgul           #+#    #+#             */
/*   Updated: 2026/10/20 15:00:00 by esakgul          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

static void	tex_fail(t_game *game, const char *path)
{
	ft_putstr_fd("Error\ncannot load texture (not a valid XPM): ",
		STDERR_FILENO);
	ft_putendl_fd((char *)path, STDERR_FILENO);
	free_game(game);
	exit(EXIT_FAILURE);
}

/*
** mlx_xpm_file_to_image parses the file and returns an image plus its
** size; mlx_get_data_addr then exposes the raw pixel buffer. Runs after
** mlx_init and BEFORE the window exists: a missing or malformed file
** prints "Error\n..." and exits with nothing on screen. The parser already
** checked that each path is a readable .xpm; this is the content check.
*/
void	load_textures(t_game *game)
{
	t_tex	*t;
	int		i;

	i = 0;
	while (i < 4)
	{
		t = &game->tex[i];
		t->path = game->map.tex_path[i];
		t->img = mlx_xpm_file_to_image(game->mlx, t->path, &t->w, &t->h);
		if (!t->img)
			tex_fail(game, t->path);
		t->addr = mlx_get_data_addr(t->img, &t->bpp, &t->line_len,
				&t->endian);
		if (!t->addr || t->bpp != 32 || t->w <= 0 || t->h <= 0)
			tex_fail(game, t->path);
		i++;
	}
}

/* Same offset maths as put_pixel, reading instead of writing.            */
int	get_texel(t_tex *tex, int x, int y)
{
	char	*src;

	if (x < 0)
		x = 0;
	if (y < 0)
		y = 0;
	if (x >= tex->w)
		x = tex->w - 1;
	if (y >= tex->h)
		y = tex->h - 1;
	src = tex->addr + y * tex->line_len + x * (tex->bpp / 8);
	return ((int)(*(unsigned int *)src));
}

/*
** wall_x: where along the wall block the ray hit, 0..1. For an x-side hit
** the wall runs along y, so take the y coordinate of the hit point
** (pos + perp * dir), and vice versa; subtract the integer part.
** tex_x = wall_x * width, then mirror when the screen's left-to-right
** order runs against the world axis wall_x is measured on. With our
** plane = (-dir_y, dir_x) convention the leftmost column looks to the
** player's left, so:
**   facing west  (side 0, dir_x < 0): left of screen = larger y -> flip
**   facing south (side 1, dir_y > 0): left of screen = larger x -> flip
** (Lodev flips the opposite two cases because his plane sign is opposite.)
*/
void	tex_column(t_game *game, t_ray *ray)
{
	t_tex	*tex;

	tex = &game->tex[ray->face];
	if (ray->side == 0)
		ray->wall_x = game->player.y + ray->perp * ray->dir_y;
	else
		ray->wall_x = game->player.x + ray->perp * ray->dir_x;
	ray->wall_x -= floor(ray->wall_x);
	ray->tex_x = (int)(ray->wall_x * tex->w);
	if ((ray->side == 0 && ray->dir_x < 0)
		|| (ray->side == 1 && ray->dir_y > 0))
		ray->tex_x = tex->w - ray->tex_x - 1;
}
