/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_bonus.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ogokdas <ogokdas@student.42istanbul.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/22 17:21:29 by ogokdas           #+#    #+#             */
/*   Updated: 2026/11/22 17:21:29 by ogokdas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d_bonus.h"

/* Same start-up order as the mandatory main, with the bonus hooks.       */
int	main(int argc, char **argv)
{
	t_bgame	bg;

	ft_bzero(&bg, sizeof(t_bgame));
	if (argc != 2)
		exit_error(&bg.g, "usage: ./cub3D <scene.cub>");
	parse_cub(&bg.g, argv[1]);
	if (!init_mlx(&bg.g))
		exit_error(&bg.g, "miniLibX initialisation failed");
	load_textures(&bg.g);
	if (!init_window(&bg.g))
		exit_error(&bg.g, "miniLibX window creation failed");
	if (!init_frame(&bg.g))
		exit_error(&bg.g, "miniLibX image creation failed");
	set_hooks_bonus(&bg);
	mlx_loop(bg.g.mlx);
	free_game(&bg.g);
	return (0);
}
