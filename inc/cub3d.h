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
# include <sys/time.h>
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

/* Identifier indexes 4 and 5 (after the four sides) are the colours.      */
# define ID_F				4
# define ID_C				5
# define ID_COUNT			6
# define ERR_DETAIL_LEN		256

/* Camera plane half-length: tan(66deg / 2) ~= 0.66 gives a 66deg FOV.     */
# define FOV_PLANE			0.66

/* Minimum time between two rendered frames (ms): ~60 fps.                 */
# define FRAME_MS			16

/*
** Movement is time based: cells per second and radians per second, scaled
** by the measured frame delta so speed is the same on any machine.
** MAX_DT caps the delta after a stall (window dragged, minimised) so the
** player never teleports through a wall in one frame.
*/
# define MOVE_SPEED			3.0
# define ROT_SPEED			2.0
# define MAX_DT				0.1
/* Distance kept between the camera and any wall (in cells).              */
# define WALL_MARGIN		0.15

/* Phase 4 flat wall colours: x-side hits lighter, y-side hits darker.     */
# define WALL_X_COLOR		0xC8C8C8
# define WALL_Y_COLOR		0x7A7A7A
/* Substitute for 1/0 when a ray component is exactly zero.                */
# define RAY_INF			1e30

/* Indexes into t_game.keys: the keys whose held state matters.            */
enum e_key
{
	K_W = 0,
	K_A = 1,
	K_S = 2,
	K_D = 3,
	K_LEFT = 4,
	K_RIGHT = 5,
	K_COUNT = 6
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
	int		floor_set;
	int		ceil_set;
	char	spawn_dir;
}	t_map;

/*
** t_parse - parser-only state, lives on parse_cub's stack.
** Reason: the raw file lines and the pending error message are needed only
** while parsing; keeping them out of t_game means nothing to free later and
** one place (parse_fail) that prints, frees and exits.
** detail is a copy, so it may come from a buffer that is freed afterwards.
*/
typedef struct s_parse
{
	char		**lines;
	int			n;
	const char	*err;
	char		detail[ERR_DETAIL_LEN];
}	t_parse;

/*
** t_ray - everything about the ray cast for one screen column (Lodev DDA).
** Reason: the per-column state is passed between init_ray / perform_dda /
** compute_wall / draw_column; one struct keeps every function under four
** parameters and makes each step testable on its own.
**   camera_x   -1 (left edge) .. +1 (right edge) of the screen
**   dir_x/y    ray direction = player dir + plane * camera_x
**   map_x/y    grid cell the ray is currently in
**   delta_x/y  distance along the ray between two x (resp. y) grid lines
**   side_x/y   distance along the ray to the next x (resp. y) grid line
**   step_x/y   +1 or -1, which way the ray crosses cells
**   side       0 = hit an x-side (vertical wall), 1 = hit a y-side
**   perp       perpendicular distance to the wall (no fisheye)
**   face       which wall face was hit: NO / SO / WE / EA (texture index)
*/
typedef struct s_ray
{
	double	camera_x;
	double	dir_x;
	double	dir_y;
	int		map_x;
	int		map_y;
	double	delta_x;
	double	delta_y;
	double	side_x;
	double	side_y;
	int		step_x;
	int		step_y;
	int		side;
	int		hit;
	double	perp;
	int		line_h;
	int		draw_start;
	int		draw_end;
	int		face;
	double	wall_x;
	int		tex_x;
}	t_ray;

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
	int			keys[K_COUNT];
	long		last_frame;
}	t_game;

/* ---------------------------------------------------------------------- */
/*  Phase 0/3 - window, image buffer, events, cleanup (implemented)        */
/* ---------------------------------------------------------------------- */
/* window.c */
int			init_mlx(t_game *game);
int			init_window(t_game *game);
int			init_frame(t_game *game);
long		now_ms(void);
int			game_loop(t_game *game);
/* hooks.c */
int			key_index(int keycode);
int			handle_key(int keycode, t_game *game);
int			handle_release(int keycode, t_game *game);
int			handle_close(t_game *game);
void		set_hooks(t_game *game);
/* image.c */
void		put_pixel(t_tex *img, int x, int y, int color);
void		present_frame(t_game *game);
int			render_frame(t_game *game);

/* ---------------------------------------------------------------------- */
/*  Phase 4 - raycasting (implemented)                                     */
/* ---------------------------------------------------------------------- */
/* raycast.c */
void		init_ray(t_game *game, t_ray *ray, int x);
void		init_step(t_game *game, t_ray *ray);
void		perform_dda(t_game *game, t_ray *ray);
void		compute_wall(t_ray *ray);
int			wall_face(t_ray *ray);
/* render.c */
void		draw_wall(t_game *game, t_ray *ray, int x);
void		draw_column(t_game *game, t_ray *ray, int x);
void		cast_rays(t_game *game);
/* texture.c */
void		load_textures(t_game *game);
int			get_texel(t_tex *tex, int x, int y);
void		tex_column(t_game *game, t_ray *ray);
/* movement.c */
int			can_move(t_game *game, double x, double y);
void		move_player(t_game *game, double dx, double dy);
void		rotate_player(t_game *game, double angle);
void		handle_keys(t_game *game, double dt);
/* cleanup.c */
void		free_game(t_game *game);
int			close_game(t_game *game);
void		exit_error(t_game *game, const char *msg);

/* ---------------------------------------------------------------------- */
/*  Phase 1 - scene file parsing (implemented)                             */
/*  Every parser function returns 1 on success, 0 after set_err().         */
/* ---------------------------------------------------------------------- */
/* parse.c */
void		parse_cub(t_game *game, const char *path);
int			set_err(t_parse *p, const char *msg, const char *detail);
/* parse_file.c */
int			load_file(t_parse *p, const char *path);
/* parse_line.c */
int			is_blank(const char *s);
const char	*skip_ws(const char *s);
const char	*ident_name(int id);
int			get_identifier(const char *s);
int			is_map_line(const char *s);
/* parse_element.c */
int			parse_element(t_parse *p, t_map *map, char *line);
/* parse_rgb.c */
int			parse_rgb(t_parse *p, const char *s, int *out);
/* parse_map_start.c */
int			find_map_start(t_parse *p, t_map *map);
int			collect_map(t_parse *p, t_map *map, int start);
int			check_all_set(t_parse *p, t_map *map);

/* ---------------------------------------------------------------------- */
/*  Phase 2 - map validation (implemented)                                 */
/* ---------------------------------------------------------------------- */
/* map_validate.c */
int			validate_map(t_parse *p, t_game *game);
char		map_cell(t_map *map, int x, int y);
/* map_player.c */
int			find_player(t_parse *p, t_game *game);

#endif
