/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   xpm_check.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ogokdas <ogokdas@student.42istanbul.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/20 00:10:14 by ogokdas           #+#    #+#             */
/*   Updated: 2026/11/20 00:10:17 by ogokdas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

/* First bytes of the file, NUL-terminated. 0 when nothing can be read.   */
static int	read_head(const char *path, char *buf, int size)
{
	int	fd;
	int	n;

	fd = open(path, O_RDONLY);
	if (fd < 0)
		return (0);
	n = read(fd, buf, size - 1);
	close(fd);
	if (n <= 0)
		return (0);
	buf[n] = '\0';
	return (1);
}

/*
** One header number at s[*i], after optional spaces. -1 when there is no
** digit or when it has more than 8 digits (no overflow possible).
*/
static long	xpm_field(const char *s, int *i)
{
	long	v;
	int		digits;

	while (s[*i] == ' ' || s[*i] == '\t')
		(*i)++;
	if (!ft_isdigit(s[*i]))
		return (-1);
	v = 0;
	digits = 0;
	while (ft_isdigit(s[*i]))
	{
		v = v * 10 + (s[*i] - '0');
		(*i)++;
		digits++;
		if (digits > 8)
			return (-1);
	}
	return (v);
}

/*
** mlx trusts the "<width> <height> <colors> <chars_per_pixel>" string of
** an XPM: a file claiming 99999x99999 makes it map gigabytes of shared
** memory and die with SIGBUS before it notices the pixels are missing.
** So the first quoted string is checked here, before mlx sees the file:
** all four values >= 1, width and height at most XPM_MAX_SIDE.
*/
int	xpm_header_ok(const char *path)
{
	char	buf[1024];
	int		i;
	int		k;
	long	v;

	if (!read_head(path, buf, sizeof(buf)))
		return (0);
	i = 0;
	while (buf[i] && buf[i] != '"')
		i++;
	if (buf[i] != '"')
		return (0);
	i++;
	k = 0;
	while (k < 4)
	{
		v = xpm_field(buf, &i);
		if (v < 1 || (k < 2 && v > XPM_MAX_SIDE))
			return (0);
		k++;
	}
	return (1);
}
