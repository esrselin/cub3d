/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   raycast.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esakgul <esakgul@student.42istanbul.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/20 15:00:00 by esakgul           #+#    #+#             */
/*   Updated: 2026/10/20 15:00:00 by esakgul          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

/*
** Step 1-2. camera_x maps the column to [-1, 1] across the camera plane,
** so the ray is dir + plane * camera_x: leftmost column = dir - plane.
** delta_x is how far the ray travels between two vertical grid lines:
** |1 / dir_x| (by similar triangles the full ray length per unit x). A
** zero component would divide by zero; RAY_INF means "never crosses".
*/
void	init_ray(t_game *game, t_ray *ray, int x)
{
	t_player	*pl;

	pl = &game->player;
	ray->camera_x = 2.0 * x / WIN_W - 1.0;
	ray->dir_x = pl->dir_x + pl->plane_x * ray->camera_x;
	ray->dir_y = pl->dir_y + pl->plane_y * ray->camera_x;
	ray->map_x = (int)pl->x;
	ray->map_y = (int)pl->y;
	ray->delta_x = RAY_INF;
	if (ray->dir_x != 0.0)
		ray->delta_x = fabs(1.0 / ray->dir_x);
	ray->delta_y = RAY_INF;
	if (ray->dir_y != 0.0)
		ray->delta_y = fabs(1.0 / ray->dir_y);
	ray->hit = 0;
	ray->side = 0;
}

/*
** Step 3. step is the direction of travel per axis. side_x is the ray
** length from the player to the FIRST vertical grid line in that
** direction: the fractional distance to the cell edge times delta_x.
*/
void	init_step(t_game *game, t_ray *ray)
{
	t_player	*pl;

	pl = &game->player;
	if (ray->dir_x < 0)
	{
		ray->step_x = -1;
		ray->side_x = (pl->x - ray->map_x) * ray->delta_x;
	}
	else
	{
		ray->step_x = 1;
		ray->side_x = (ray->map_x + 1.0 - pl->x) * ray->delta_x;
	}
	if (ray->dir_y < 0)
	{
		ray->step_y = -1;
		ray->side_y = (pl->y - ray->map_y) * ray->delta_y;
	}
	else
	{
		ray->step_y = 1;
		ray->side_y = (ray->map_y + 1.0 - pl->y) * ray->delta_y;
	}
}

/*
** Step 4. Always cross the NEAREST grid line next (smaller side_dist),
** then extend that side_dist by one delta. side remembers which kind of
** line was crossed last. A space (void / outside the grid) also stops the
** ray: the map is closed, but this keeps a corrupt grid from looping.
*/
void	perform_dda(t_game *game, t_ray *ray)
{
	char	c;

	while (!ray->hit)
	{
		if (ray->side_x < ray->side_y)
		{
			ray->side_x += ray->delta_x;
			ray->map_x += ray->step_x;
			ray->side = 0;
		}
		else
		{
			ray->side_y += ray->delta_y;
			ray->map_y += ray->step_y;
			ray->side = 1;
		}
		c = map_cell(&game->map, ray->map_x, ray->map_y);
		if (c == '1' || c == ' ')
			ray->hit = 1;
	}
}

/*
** Step 5-6. After the loop side_x already points one delta PAST the wall,
** so side_x - delta_x is the ray length to the wall; because delta was
** built from |1/dir| this length is measured along the camera direction,
** i.e. it is the perpendicular distance. Using the Euclidean distance
** instead would curve flat walls (fisheye). line_h = WIN_H / perp, centred.
*/
void	compute_wall(t_ray *ray)
{
	if (ray->side == 0)
		ray->perp = ray->side_x - ray->delta_x;
	else
		ray->perp = ray->side_y - ray->delta_y;
	if (ray->perp < 1e-6)
		ray->perp = 1e-6;
	ray->line_h = (int)(WIN_H / ray->perp);
	ray->draw_start = WIN_H / 2 - ray->line_h / 2;
	if (ray->draw_start < 0)
		ray->draw_start = 0;
	ray->draw_end = WIN_H / 2 + ray->line_h / 2;
	if (ray->draw_end >= WIN_H)
		ray->draw_end = WIN_H - 1;
	ray->face = wall_face(ray);
}

/*
** Step 8. Which face of the wall block is turned toward the player:
**   side 0, dir_x > 0  -> ray travels east, sees the block's west face: EA
**   side 0, dir_x < 0  -> WE
**   side 1, dir_y > 0  -> travels south (y grows downward): SO
**   side 1, dir_y < 0  -> NO
** Convention: "NO" is the wall you see when you look north.
*/
int	wall_face(t_ray *ray)
{
	if (ray->side == 0)
	{
		if (ray->dir_x > 0)
			return (EA);
		return (WE);
	}
	if (ray->dir_y > 0)
		return (SO);
	return (NO);
}
