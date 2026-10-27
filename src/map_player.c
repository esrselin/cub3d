/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   map_player.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esakgul <esakgul@student.42istanbul.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/20 15:00:00 by esakgul           #+#    #+#             */
/*   Updated: 2026/10/20 15:00:00 by esakgul          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

/*
** Grid rows grow downward, so "north" is -y. The camera plane is the
** direction vector rotated a quarter turn, scaled by tan(FOV / 2):
** plane = (-dir_y, dir_x) * FOV_PLANE. With this sign the leftmost screen
** column looks to the player's left (not mirrored).
**   N: dir ( 0,-1) plane ( 0.66, 0   )
**   S: dir ( 0, 1) plane (-0.66, 0   )
**   E: dir ( 1, 0) plane ( 0,    0.66)
**   W: dir (-1, 0) plane ( 0,   -0.66)
*/
static void	set_orientation(t_player *pl, char dir)
{
	pl->dir_x = 0;
	pl->dir_y = 0;
	if (dir == 'N')
		pl->dir_y = -1;
	else if (dir == 'S')
		pl->dir_y = 1;
	else if (dir == 'E')
		pl->dir_x = 1;
	else
		pl->dir_x = -1;
	pl->plane_x = -pl->dir_y * FOV_PLANE;
	pl->plane_y = pl->dir_x * FOV_PLANE;
}

/*
** Counts N/S/E/W cells; records the last one seen and turns it into a
** plain '0' so the renderer and collision code only ever see '0' and '1'.
*/
static int	count_players(t_game *game)
{
	int	x;
	int	y;
	int	count;

	count = 0;
	y = 0;
	while (y < game->map.h)
	{
		x = 0;
		while (x < game->map.w)
		{
			if (ft_strchr("NSEW", game->map.grid[y][x]))
			{
				count++;
				game->map.spawn_dir = game->map.grid[y][x];
				game->map.grid[y][x] = '0';
				game->player.x = x + 0.5;
				game->player.y = y + 0.5;
			}
			x++;
		}
		y++;
	}
	return (count);
}

/* Exactly one spawn; the player starts at the centre of its cell.         */
int	find_player(t_parse *p, t_game *game)
{
	int	count;

	count = count_players(game);
	if (count == 0)
		return (set_err(p, "map has no player start (N/S/E/W)", NULL));
	if (count > 1)
		return (set_err(p, "map has more than one player start", NULL));
	set_orientation(&game->player, game->map.spawn_dir);
	return (1);
}
