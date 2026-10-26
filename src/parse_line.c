/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_line.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esakgul <esakgul@student.42istanbul.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/20 15:00:00 by esakgul           #+#    #+#             */
/*   Updated: 2026/10/20 15:00:00 by esakgul          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

int	is_blank(const char *s)
{
	while (*s == ' ' || *s == '\t')
		s++;
	return (*s == '\0');
}

const char	*skip_ws(const char *s)
{
	while (*s == ' ' || *s == '\t')
		s++;
	return (s);
}

/* Index -> identifier text, same order as enum e_side then ID_F, ID_C.   */
const char	*ident_name(int id)
{
	if (id == NO)
		return ("NO");
	if (id == SO)
		return ("SO");
	if (id == WE)
		return ("WE");
	if (id == EA)
		return ("EA");
	if (id == ID_F)
		return ("F");
	return ("C");
}

/*
** Returns 0..5 when the line starts (after whitespace) with an identifier
** that is followed by whitespace or end of line, otherwise -1.
** "NO ./a.xpm" -> 0, "NO" -> 0, "NO./a.xpm" -> -1, "N001" -> -1.
*/
int	get_identifier(const char *s)
{
	const char	*name;
	size_t		len;
	int			i;

	s = skip_ws(s);
	i = 0;
	while (i < ID_COUNT)
	{
		name = ident_name(i);
		len = ft_strlen(name);
		if (ft_strncmp(s, name, len) == 0
			&& (s[len] == ' ' || s[len] == '\t' || s[len] == '\0'))
			return (i);
		i++;
	}
	return (-1);
}

/*
** A map line is non-blank, made only of map characters and whitespace, and
** is not an element line ("NO" alone would otherwise pass). Lines such as
** "NO./a.xpm" or "NORTH ./a.xpm" fail here and reach parse_element, which
** reports them as unknown identifiers. Phase 2 validates cell semantics.
*/
int	is_map_line(const char *s)
{
	const char	*c;

	c = skip_ws(s);
	if (*c == '\0' || get_identifier(c) >= 0)
		return (0);
	while (*c)
	{
		if (!ft_strchr("01NSEW \t", *c))
			return (0);
		c++;
	}
	return (1);
}
