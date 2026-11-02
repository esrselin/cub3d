/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esakgul <esakgul@student.42istanbul.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/20 15:00:00 by esakgul           #+#    #+#             */
/*   Updated: 2026/10/20 15:00:00 by esakgul          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

static void	free_map(t_map *map)
{
	int	i;

	i = 0;
	while (i < 4)
	{
		free(map->tex_path[i]);
		map->tex_path[i] = NULL;
		i++;
	}
	ft_free_tab(map->grid);
	map->grid = NULL;
}

/* Images need the display, so they go before the window and the display. */
static void	free_images(t_game *game)
{
	int	i;

	if (game->frame.img)
		mlx_destroy_image(game->mlx, game->frame.img);
	game->frame.img = NULL;
	i = 0;
	while (i < 4)
	{
		if (game->tex[i].img)
			mlx_destroy_image(game->mlx, game->tex[i].img);
		game->tex[i].img = NULL;
		i++;
	}
}

/*
** Shutdown order: images -> window -> display -> free(mlx) -> own data.
** mlx_destroy_display exists only in the Linux minilibx; the macOS one
** has no display handle to release.
*/
void	free_game(t_game *game)
{
	free_map(&game->map);
	if (game->mlx)
		free_images(game);
	if (game->win)
		mlx_destroy_window(game->mlx, game->win);
	if (game->mlx)
	{
		mlx_destroy_display(game->mlx);
		free(game->mlx);
	}
	game->win = NULL;
	game->mlx = NULL;
}

int	close_game(t_game *game)
{
	free_game(game);
	exit(EXIT_SUCCESS);
	return (0);
}

void	exit_error(t_game *game, const char *msg)
{
	ft_putstr_fd("Error\n", STDERR_FILENO);
	if (msg)
		ft_putendl_fd((char *)msg, STDERR_FILENO);
	free_game(game);
	exit(EXIT_FAILURE);
}
