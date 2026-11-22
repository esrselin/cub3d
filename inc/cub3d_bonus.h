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

/* X11 pointer events / masks (see X11/X.h).                              */
# define EV_MOTION			6
# define EV_LEAVE			8
# define MASK_MOTION		64
# define MASK_LEAVE			32
/* Radians turned per pixel of horizontal mouse movement.                 */
# define MOUSE_SENS			0.003

/*
** t_bgame - root object of the bonus build.
** Reason: t_game is the FIRST member, so a t_bgame * is also a valid
** t_game * (C guarantees a struct and its first member share an address).
** Every mandatory function (parser, raycaster, movement, cleanup) is
** reused unchanged on &bg->g; only the bonus state lives here.
**   mouse_x    pointer x at the previous MotionNotify
**   mouse_in   0 until the first MotionNotify after entering the window
*/
typedef struct s_bgame
{
	t_game	g;
	int		mouse_x;
	int		mouse_in;
}	t_bgame;

/* loop_bonus.c */
int		game_loop_bonus(t_bgame *bg);
void	set_hooks_bonus(t_bgame *bg);
/* mouse_bonus.c */
int		handle_mouse(int x, int y, t_bgame *bg);
int		handle_mouse_leave(t_bgame *bg);

#endif
