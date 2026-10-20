/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cub3d.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esakgul <esakgul@student.42istanbul.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/20 15:00:00 by esakgul           #+#    #+#             */
/*   Updated: 2026/10/20 15:00:00 by esakgul          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CUB3D_H
# define CUB3D_H

# include <stdlib.h>
# include <unistd.h>
# include <fcntl.h>
# include <stdio.h>
# include <math.h>
# include "mlx.h"
# include "libft.h"

/* ---------------------------------------------------------------------- */
/*  Window                                                                 */
/* ---------------------------------------------------------------------- */
# define WIN_W				1024
# define WIN_H				768
# define WIN_TITLE			"cub3D"

/* ---------------------------------------------------------------------- */
/*  X11 events / masks (see X11/X.h). mlx_hook(win, event, mask, f, arg)   */
/* ---------------------------------------------------------------------- */
# define EV_KEY_PRESS		2
# define EV_KEY_RELEASE		3
# define EV_DESTROY			17
# define MASK_KEY_PRESS		(1L << 0)
# define MASK_KEY_RELEASE	(1L << 1)
# define MASK_STRUCTURE		(1L << 17)

/* ---------------------------------------------------------------------- */
/*  X11 keysyms (Linux, see X11/keysymdef.h)                               */
/* ---------------------------------------------------------------------- */
# define KEY_ESC			65307
# define KEY_W				119
# define KEY_A				97
# define KEY_S				115
# define KEY_D				100
# define KEY_LEFT			65361
# define KEY_RIGHT			65363

/* Wall orientations, in the order the .cub identifiers are listed.        */
enum e_side
{
	NO = 0,
	SO = 1,
	WE = 2,
	EA = 3
};

/*
** t_tex - one mlx image and the raw pixel buffer behind it.
** Reason: both the on-screen frame buffer and the four wall textures are
** "an image we read/write pixel by pixel". Keeping img + addr + bpp +
** line_len + endian together means one put_pixel/get_pixel helper serves
** every image in the program.
*/
typedef struct s_tex
{
	void	*img;
	char	*addr;
	int		w;
	int		h;
	int		bpp;
	int		line_len;
	int		endian;
	char	*path;
}	t_tex;

/*
** t_player - position and camera of the player.
** Reason: raycasting needs a position (x, y), a direction vector and a
** camera plane vector; movement/rotation only ever touch these six
** doubles, so they live apart from the map and the window.
*/
typedef struct s_player
{
	double	x;
	double	y;
	double	dir_x;
	double	dir_y;
	double	plane_x;
	double	plane_y;
}	t_player;

/*
** t_map - everything parsed from the .cub file.
** Reason: the parser (phase 1-2) owns this struct and fills it; the
** renderer only reads it. Separating it makes the parser testable without
** opening a window.
*/
typedef struct s_map
{
	char	**grid;
	int		w;
	int		h;
	char	*tex_path[4];
	int		floor_rgb;
	int		ceil_rgb;
	char	spawn_dir;
}	t_map;

/*
** t_game - root object passed to every mlx callback as the `void *param`.
** Reason: mlx hooks accept a single user pointer, so every subsystem the
** callbacks need (window, frame, map, player, textures) hangs off here.
** It also makes cleanup trivial: free_game() walks this one struct.
*/
typedef struct s_game
{
	void		*mlx;
	void		*win;
	t_tex		frame;
	t_tex		tex[4];
	t_map		map;
	t_player	player;
}	t_game;

/* ---------------------------------------------------------------------- */
/*  Phase 0 - window / events / cleanup (implemented)                      */
/* ---------------------------------------------------------------------- */
void	set_hooks(t_game *game);
int		handle_key(int keycode, t_game *game);
int		handle_close(t_game *game);
void	free_game(t_game *game);
int		close_game(t_game *game);
void	exit_error(t_game *game, const char *msg);

/* ---------------------------------------------------------------------- */
/*  Phase 1-2 - parsing & validation (to be implemented)                   */
/* ---------------------------------------------------------------------- */
int		parse_cub(t_game *game, const char *path);
int		validate_map(t_game *game);

/* ---------------------------------------------------------------------- */
/*  Phase 3-5 - images, raycasting, textures (to be implemented)           */
/* ---------------------------------------------------------------------- */
int		init_frame(t_game *game);
int		load_textures(t_game *game);
int		render_frame(t_game *game);
void	put_pixel(t_tex *img, int x, int y, int color);

/* ---------------------------------------------------------------------- */
/*  Phase 6 - movement (to be implemented)                                 */
/* ---------------------------------------------------------------------- */
void	move_player(t_game *game, double dx, double dy);
void	rotate_player(t_game *game, double angle);

#endif
