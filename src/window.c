/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   window.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esakgul <esakgul@student.42istanbul.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/20 15:00:00 by esakgul           #+#    #+#             */
/*   Updated: 2026/10/20 15:00:00 by esakgul          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

/* Connects to the X server. Needed before textures can be loaded.        */
int	init_mlx(t_game *game)
{
	game->mlx = mlx_init();
	if (!game->mlx)
		return (0);
	return (1);
}

/* Opened only once every texture is loaded, so a bad XPM never shows a
** window that immediately disappears.                                    */
int	init_window(t_game *game)
{
	game->win = mlx_new_window(game->mlx, WIN_W, WIN_H, WIN_TITLE);
	if (!game->win)
		return (0);
	return (1);
}

/*
** One off-screen image the size of the window. Every frame is drawn into
** its buffer and pushed with a single mlx_put_image_to_window. bpp is
** asserted to be 32 so a pixel is exactly one unsigned int.
*/
int	init_frame(t_game *game)
{
	t_tex	*f;

	f = &game->frame;
	f->w = WIN_W;
	f->h = WIN_H;
	f->img = mlx_new_image(game->mlx, WIN_W, WIN_H);
	if (!f->img)
		return (0);
	f->addr = mlx_get_data_addr(f->img, &f->bpp, &f->line_len, &f->endian);
	if (!f->addr || f->bpp != 32)
		return (0);
	return (1);
}

long	now_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000L + tv.tv_usec / 1000L);
}

/*
** mlx_loop calls this continuously whenever no X event is pending.
** Rendering only every FRAME_MS keeps the per-frame work bounded; the
** loop itself still spins because the subject does not allow usleep.
*/
int	game_loop(t_game *game)
{
	long	now;

	now = now_ms();
	if (now - game->last_frame < FRAME_MS)
		return (0);
	game->last_frame = now;
	render_frame(game);
	return (0);
}
