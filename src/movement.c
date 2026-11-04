/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   movement.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esakgul <esakgul@student.42istanbul.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/20 15:00:00 by esakgul           #+#    #+#             */
/*   Updated: 2026/10/20 15:00:00 by esakgul          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

/* A position is free when its cell is floor; anything else blocks.       */
int	can_move(t_game *game, double x, double y)
{
	return (map_cell(&game->map, (int)x, (int)y) == '0');
}

/*
** The x and y displacements are tested separately: if the x step is
** blocked but the y step is free, the player still slides along the wall
** instead of stopping dead. The test point is pushed WALL_MARGIN ahead in
** the direction of travel so the camera never sits exactly on a wall
** plane (perp distance 0 -> infinite line height).
*/
void	move_player(t_game *game, double dx, double dy)
{
	t_player	*pl;
	double		probe;

	pl = &game->player;
	probe = WALL_MARGIN;
	if (dx < 0)
		probe = -WALL_MARGIN;
	if (dx != 0 && can_move(game, pl->x + dx + probe, pl->y))
		pl->x += dx;
	probe = WALL_MARGIN;
	if (dy < 0)
		probe = -WALL_MARGIN;
	if (dy != 0 && can_move(game, pl->x, pl->y + dy + probe))
		pl->y += dy;
}

/*
** 2D rotation by `angle`: x' = x cos a - y sin a, y' = x sin a + y cos a.
** With y pointing down a positive angle turns clockwise on screen, i.e.
** to the right. The camera plane is rotated by the same matrix so it stays
** perpendicular to dir; rotating only dir would skew the whole view.
** The matrix is orthonormal, so |dir| and |plane| are preserved up to
** floating point noise (1e-16 per call): no normalisation is needed.
*/
void	rotate_player(t_game *game, double angle)
{
	t_player	*pl;
	double		old_x;
	double		c;
	double		s;

	pl = &game->player;
	c = cos(angle);
	s = sin(angle);
	old_x = pl->dir_x;
	pl->dir_x = pl->dir_x * c - pl->dir_y * s;
	pl->dir_y = old_x * s + pl->dir_y * c;
	old_x = pl->plane_x;
	pl->plane_x = pl->plane_x * c - pl->plane_y * s;
	pl->plane_y = old_x * s + pl->plane_y * c;
}

/*
** Called once per rendered frame with the elapsed time. Every held key
** contributes to one displacement vector and one rotation angle, so W +
** Right or W + A act together. Forward/back follow dir; strafing follows
** the unit vector perpendicular to dir: left = (dir_y, -dir_x), which for
** dir = north (0,-1) gives (-1, 0) = west.
*/
void	handle_keys(t_game *game, double dt)
{
	t_player	*pl;
	double		fwd;
	double		side;
	double		rot;

	pl = &game->player;
	fwd = (game->keys[K_W] - game->keys[K_S]) * MOVE_SPEED * dt;
	side = (game->keys[K_D] - game->keys[K_A]) * MOVE_SPEED * dt;
	if (fwd != 0 || side != 0)
		move_player(game, pl->dir_x * fwd - pl->dir_y * side,
			pl->dir_y * fwd + pl->dir_x * side);
	rot = (game->keys[K_RIGHT] - game->keys[K_LEFT]) * ROT_SPEED * dt;
	if (rot != 0)
		rotate_player(game, rot);
}
