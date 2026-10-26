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

void	free_game(t_game *game)
{
	free_map(&game->map);
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
