/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_rgb.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esakgul <esakgul@student.42istanbul.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/20 15:00:00 by esakgul           #+#    #+#             */
/*   Updated: 2026/10/20 15:00:00 by esakgul          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

/*
** Reads 1 to 3 digits at s[*i] and advances *i past them.
** Returns the value (0..255), -1 for "no digit" or "4+ digits",
** -2 for a value above 255.
*/
static int	parse_channel(const char *s, int *i)
{
	int	val;
	int	digits;

	if (!ft_isdigit(s[*i]))
		return (-1);
	val = 0;
	digits = 0;
	while (ft_isdigit(s[*i]))
	{
		val = val * 10 + (s[*i] - '0');
		digits++;
		if (digits > 3)
			return (-1);
		(*i)++;
	}
	if (val > 255)
		return (-2);
	return (val);
}

/*
** Strict "R,G,B": three channels, exactly one comma between them, nothing
** after the last one. Rejects "255,,0", "255,0,0,", "a,b,c", "-1,0,0",
** "0255,0,0" (invalid) and "256,0,0" (out of range).
*/
int	parse_rgb(t_parse *p, const char *s, int *out)
{
	int	i;
	int	c;
	int	v[3];

	i = 0;
	c = 0;
	while (c < 3)
	{
		v[c] = parse_channel(s, &i);
		if (v[c] == -2)
			return (set_err(p, "color channel out of range (0-255)", s));
		if (v[c] < 0 || (c < 2 && s[i] != ','))
			return (set_err(p, "invalid color value", s));
		if (c < 2)
			i++;
		c++;
	}
	if (s[i] != '\0')
		return (set_err(p, "invalid color value", s));
	*out = (v[0] << 16) | (v[1] << 8) | v[2];
	return (1);
}
