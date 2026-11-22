/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   loop_bonus.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ogokdas <ogokdas@student.42istanbul.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/22 17:21:30 by ogokdas           #+#    #+#             */
/*   Updated: 2026/11/22 17:40:11 by ogokdas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d_bonus.h"

/*
** Same frame cap and time-based movement as the mandatory game_loop.
** The frame is built in steps (3D view, then the bonus overlays) and
** pushed to the window once, so overlays never flicker.
*/
int	game_loop_bonus(t_bgame *bg)
{
	t_game	*game;
	long	now;
	double	dt;

	game = &bg->g;
	now = now_ms();
	if (now - game->last_frame < FRAME_MS)
		return (0);
	dt = (now - game->last_frame) / 1000.0;
	if (dt > MAX_DT)
		dt = MAX_DT;
	game->last_frame = now;
	handle_keys(game, dt);
	cast_rays(game);
	draw_minimap(bg);
	draw_minimap_player(bg);
	present_frame(game);
	return (0);
}

/*
** The keyboard and close handlers are the mandatory ones; the pointer
** hooks get the whole t_bgame because they need the mouse state.
*/
void	set_hooks_bonus(t_bgame *bg)
{
	t_game	*game;

	game = &bg->g;
	mlx_hook(game->win, EV_KEY_PRESS, MASK_KEY_PRESS, handle_key, game);
	mlx_hook(game->win, EV_KEY_RELEASE, MASK_KEY_RELEASE,
		handle_release, game);
	mlx_hook(game->win, EV_DESTROY, MASK_STRUCTURE, handle_close, game);
	mlx_hook(game->win, EV_MOTION, MASK_MOTION, handle_mouse, bg);
	mlx_hook(game->win, EV_LEAVE, MASK_LEAVE, handle_mouse_leave, bg);
	mlx_loop_hook(game->mlx, game_loop_bonus, bg);
}
