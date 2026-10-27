/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esakgul <esakgul@student.42istanbul.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/20 15:00:00 by esakgul           #+#    #+#             */
/*   Updated: 2026/10/20 15:00:00 by esakgul          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

/*
** Records an error and returns 0 so callers can write
** `return (set_err(p, "...", tok));`. The detail is copied because it often
** points into a buffer (split tokens, file lines) that is freed before the
** message is printed.
*/
int	set_err(t_parse *p, const char *msg, const char *detail)
{
	p->err = msg;
	p->detail[0] = '\0';
	if (detail)
		ft_strlcpy(p->detail, detail, ERR_DETAIL_LEN);
	return (0);
}

static void	parse_fail(t_game *game, t_parse *p)
{
	ft_putstr_fd("Error\n", STDERR_FILENO);
	ft_putstr_fd((char *)p->err, STDERR_FILENO);
	if (p->detail[0])
	{
		ft_putstr_fd(": ", STDERR_FILENO);
		ft_putstr_fd(p->detail, STDERR_FILENO);
	}
	ft_putchar_fd('\n', STDERR_FILENO);
	ft_free_tab(p->lines);
	free_game(game);
	exit(EXIT_FAILURE);
}

/*
** Fills game->map from the scene file or exits with "Error\n<message>".
** Order: file checks -> elements until the first map line -> raw map rows
** -> all six elements present -> map grid validation (closed, one player).
*/
void	parse_cub(t_game *game, const char *path)
{
	t_parse	p;
	int		start;

	ft_bzero(&p, sizeof(t_parse));
	if (!load_file(&p, path))
		parse_fail(game, &p);
	start = find_map_start(&p, &game->map);
	if (start < 0)
		parse_fail(game, &p);
	if (!collect_map(&p, &game->map, start)
		|| !check_all_set(&p, &game->map)
		|| !validate_map(&p, game))
		parse_fail(game, &p);
	ft_free_tab(p.lines);
}
