/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cub3d_bonus.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ogokdas <ogokdas@student.42istanbul.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/22 17:21:28 by ogokdas           #+#    #+#             */
/*   Updated: 2026/11/22 17:21:28 by ogokdas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CUB3D_BONUS_H
# define CUB3D_BONUS_H

# include "cub3d.h"

/*
** t_bgame - root object of the bonus build.
** Reason: t_game is the FIRST member, so a t_bgame * is also a valid
** t_game * (C guarantees a struct and its first member share an address).
** Every mandatory function (parser, raycaster, movement, cleanup) is
** reused unchanged on &bg->g; only the bonus state lives here.
*/
typedef struct s_bgame
{
	t_game	g;
}	t_bgame;

/* loop_bonus.c */
int		game_loop_bonus(t_bgame *bg);
void	set_hooks_bonus(t_bgame *bg);

#endif
