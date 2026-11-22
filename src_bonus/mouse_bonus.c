/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mouse_bonus.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ogokdas <ogokdas@student.42istanbul.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/22 17:39:20 by ogokdas           #+#    #+#             */
/*   Updated: 2026/11/22 17:39:21 by ogokdas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d_bonus.h"

/*
** MotionNotify reports the pointer position inside the window. Only the
** horizontal change since the previous event matters: moving right turns
** right, with the same rotate_player the arrow keys use. The first event
** after the pointer enters the window only records x, so the view never
** jumps by the distance the pointer travelled outside.
*/
int	handle_mouse(int x, int y, t_bgame *bg)
{
	(void)y;
	if (bg->mouse_in && x != bg->mouse_x)
		rotate_player(&bg->g, (x - bg->mouse_x) * MOUSE_SENS);
	bg->mouse_x = x;
	bg->mouse_in = 1;
	return (0);
}

/* LeaveNotify: forget the last position until the pointer comes back.    */
int	handle_mouse_leave(t_bgame *bg)
{
	bg->mouse_in = 0;
	return (0);
}
