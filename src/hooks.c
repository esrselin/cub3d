/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hooks.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esakgul <esakgul@student.42istanbul.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/20 15:00:00 by esakgul           #+#    #+#             */
/*   Updated: 2026/10/20 15:00:00 by esakgul          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

/* X keysym -> slot in game->keys, or -1 for keys we do not track.         */
int	key_index(int keycode)
{
	if (keycode == KEY_W)
		return (K_W);
	if (keycode == KEY_A)
		return (K_A);
	if (keycode == KEY_S)
		return (K_S);
	if (keycode == KEY_D)
		return (K_D);
	if (keycode == KEY_LEFT)
		return (K_LEFT);
	if (keycode == KEY_RIGHT)
		return (K_RIGHT);
	return (-1);
}

/*
** KeyPress only records state; movement happens in game_loop so several
** keys can act at once and the speed does not depend on X autorepeat.
*/
int	handle_key(int keycode, t_game *game)
{
	int	i;

	if (keycode == KEY_ESC)
		close_game(game);
	i = key_index(keycode);
	if (i >= 0)
		game->keys[i] = 1;
	return (0);
}

int	handle_release(int keycode, t_game *game)
{
	int	i;

	i = key_index(keycode);
	if (i >= 0)
		game->keys[i] = 0;
	return (0);
}

/* DestroyNotify (event 17): the window manager's close button.           */
int	handle_close(t_game *game)
{
	close_game(game);
	return (0);
}

void	set_hooks(t_game *game)
{
	mlx_hook(game->win, EV_KEY_PRESS, MASK_KEY_PRESS, handle_key, game);
	mlx_hook(game->win, EV_KEY_RELEASE, MASK_KEY_RELEASE,
		handle_release, game);
	mlx_hook(game->win, EV_DESTROY, MASK_STRUCTURE, handle_close, game);
	mlx_loop_hook(game->mlx, game_loop, game);
}
