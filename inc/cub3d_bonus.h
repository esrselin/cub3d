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
** Minimap: a panel in the top-left corner of the frame, at most MM_SIZE
** pixels wide and high (plus MM_PAD of border). A map cell is drawn
** MM_CELL_MAX pixels at most, MM_CELL_MIN at least.
*/
# define MM_X				16
# define MM_Y				16
# define MM_SIZE			200
# define MM_PAD				4
# define MM_CELL_MAX		14
# define MM_CELL_MIN		4
# define MM_BG				0x101010
# define MM_WALL			0xD8D8D8
# define MM_FLOOR			0x404040

/*
** t_bgame - root object of the bonus build.
** Reason: t_game is the FIRST member, so a t_bgame * is also a valid
** t_game * (C guarantees a struct and its first member share an address).
** Every mandatory function (parser, raycaster, movement, cleanup) is
** reused unchanged on &bg->g; only the bonus state lives here.
**   mouse_x    pointer x at the previous MotionNotify
**   mouse_in   0 until the first MotionNotify after entering the window
**   mm_cell    minimap pixels per map cell, chosen from the map size
*/
typedef struct s_bgame
{
	t_game	g;
	int		mouse_x;
	int		mouse_in;
	int		mm_cell;
}	t_bgame;

/* loop_bonus.c */
int		game_loop_bonus(t_bgame *bg);
void	set_hooks_bonus(t_bgame *bg);
/* mouse_bonus.c */
int		handle_mouse(int x, int y, t_bgame *bg);
int		handle_mouse_leave(t_bgame *bg);
/* minimap_bonus.c */
void	draw_minimap(t_bgame *bg);

#endif
