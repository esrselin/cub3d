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

/* Identifier indexes 4 and 5 (after the four sides) are the colours.      */
# define ID_F				4
# define ID_C				5
# define ID_COUNT			6
# define ERR_DETAIL_LEN		256

/* Camera plane half-length: tan(66deg / 2) ~= 0.66 gives a 66deg FOV.     */
# define FOV_PLANE			0.66

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
