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

int	handle_key(int keycode, t_game *game)
{
	if (keycode == KEY_ESC)
		close_game(game);
	return (0);
}

int	handle_close(t_game *game)
{
	close_game(game);
	return (0);
}

void	set_hooks(t_game *game)
{
	mlx_hook(game->win, EV_KEY_PRESS, MASK_KEY_PRESS, handle_key, game);
	mlx_hook(game->win, EV_DESTROY, MASK_STRUCTURE, handle_close, game);
}
