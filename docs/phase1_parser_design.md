# Phase 1 — `.cub` scene parser design (everything except the map grid)

Scope: argv check, file reading, the six elements (NO/SO/WE/EA/F/C), detection of
where the map starts, and collecting the raw map lines for Phase 2. Map *validation*
(closed walls, single player, characters) is Phase 2.

Conventions used below
- Whitespace inside an element line = space or tab (tabs are converted to spaces
  before splitting). Rationale: the subject says "one or more spaces"; accepting tabs
  costs one helper and avoids a pointless failure on an editor that inserts tabs.
- "blank line" = empty, or only spaces/tabs.
- Every error path prints `Error\n` + message on stderr, frees everything, exits 1.

---

## 1. Error catalogue (message printed after `Error\n`)

### File level
| # | Case | Message |
|---|------|---------|
| F1 | argc != 2 | `usage: ./cub3D <map.cub>` |
| F2 | name does not end in `.cub` (`map.cu`, `map.cub.txt`, `cub`) | `scene file must have a .cub extension` |
| F3 | name is exactly `.cub` (no basename) | `scene file must have a .cub extension` |
| F4 | open() fails (missing, no permission) | `cannot open scene file: <path>` |
| F5 | path is a directory (open succeeds, read fails with EISDIR) | `cannot read scene file: <path>` |
| F6 | file is empty / only blank lines | `scene file is empty` |
| F7 | read() returns -1 mid-file | `cannot read scene file: <path>` |

### Identifier level
| # | Case | Message |
|---|------|---------|
| I1 | first token is not NO/SO/WE/EA/F/C and line is not a map line (`XX ./a.xpm`, `no ./a.xpm`, `NORTH ./a.xpm`) | `unknown identifier: <token>` |
| I2 | identifier repeated (`NO` twice) | `duplicate identifier: NO` |
| I3 | identifier with no value (`NO`, `F`) | `missing value for identifier: F` |
| I4 | identifier with more than one value token (`NO ./a.xpm extra`, `F 220, 100, 0`) | `too many values for identifier: NO` |
| I5 | identifier glued to its value (`NO./a.xpm`) → falls into I1 | `unknown identifier: NO./a.xpm` |
| I6 | one or more of the six elements never set | `missing element: SO` (first missing in NO,SO,WE,EA,F,C order) |

### Color level (`F`/`C`)
| # | Case | Message |
|---|------|---------|
| C1 | non-digit character (`a,b,c`, `25x,0,0`) | `invalid color value: <token>` |
| C2 | missing channel (`255,0`, `255`) | `invalid color value: <token>` |
| C3 | empty channel (`255,,0`, `,0,0`) | `invalid color value: <token>` |
| C4 | trailing comma / extra channel (`255,0,0,`, `1,2,3,4`) | `invalid color value: <token>` |
| C5 | channel > 255 (`256,0,0`) | `color channel out of range (0-255): <token>` |
| C6 | sign characters (`-1,0,0`, `+5,0,0`) | `invalid color value: <token>` |
| C7 | more than 3 digits in a channel (`0255,0,0`) | `invalid color value: <token>` |
| C8 | spaces inside the triplet (`220, 100, 0`) → split into >1 token → I4 | `too many values for identifier: F` |

One message for C1–C4, C6, C7 is deliberate: the user sees the offending token, and the
code stays one function with one failure path (Norm: 25 lines).

### Texture path level
| # | Case | Message |
|---|------|---------|
| T1 | file cannot be opened | `cannot open texture: <path>` |
| T2 | path is a directory (open OK, read fails) | `cannot read texture: <path>` |
| T3 | extension is not `.xpm` | `texture must be a .xpm file: <path>` |
| T4 | empty file (0 bytes) | `texture file is empty: <path>` |

Checking only "openable" would let `NO /tmp` through and crash later inside
`mlx_xpm_file_to_image`. T2/T4 are cheap and make the failure explicit.

### Ordering level
| # | Case | Message |
|---|------|---------|
| O1 | an element line appears after the first map line | `element found after map start: <identifier>` |
| O2 | no map line at all (file has only elements) | `missing map` |
| O3 | map starts before all six elements are set and nothing follows → I6 | `missing element: C` |
| O4 | a line after map start that is neither blank, map-like nor an element (`hello`) | `invalid line in map section: <line>` |

Count: 7 + 6 + 8 + 4 + 4 = 29 cases.

---

## 2. Data structures

`t_map` (already in `inc/cub3d.h`, add the two flags):

```c
typedef struct s_map
{
	char	*tex_path[4];	/* index = enum e_side (NO,SO,WE,EA); NULL = not set */
	int		floor_rgb;		/* 0x00RRGGBB */
	int		ceil_rgb;		/* 0x00RRGGBB */
	int		floor_set;		/* 0/1: colour 0,0,0 is legal, so a flag is needed */
	int		ceil_set;
	char	**grid;			/* raw map lines, NULL-terminated, as read (Phase 2 pads) */
	int		w;				/* filled in Phase 2 */
	int		h;				/* filled in Phase 2 */
	char	spawn_dir;		/* filled in Phase 2 */
}	t_map;
```

Why no "set" flag for textures: a set texture is a non-NULL pointer, the pointer is the
flag. Colours need an explicit flag because every int value is a legal colour.

Parsing state is **not** stored in `t_game`. The whole file is read once into a
`char **lines` array that lives only inside `parse_cub`. Reasons: the file is small, a
line array makes "look at line i" trivial, and freeing on error is one `ft_free_tab`.

Error propagation pattern (keeps every function under 25 lines and leak-free):

```c
const char	*err;		/* NULL = ok, otherwise a string literal to print */

err = parse_element(&g->map, lines[i]);
if (err)
{
	ft_free_tab(lines);
	exit_error(g, err);	/* exit_error already frees g->map via free_game */
}
```

Messages that need the offending token (`unknown identifier: XX`) are built with a
small fixed buffer in `exit_error`'s caller, or simpler: print the static part through
`exit_error` after writing the token yourself. Recommended: give `exit_error` a second
optional detail string:

```c
void	exit_error(t_game *g, const char *msg, const char *detail);
/* prints "Error\n<msg>" + (detail ? ": " detail : "") + "\n" */
```

`free_game` must now also free `map.tex_path[0..3]` and `ft_free_tab(map.grid)`.

---

## 3. Function-by-function design

### `src/parse_file.c` — getting the lines

| Function | Signature | Does |
|---|---|---|
| `check_extension` | `const char *check_extension(const char *path)` | Returns error msg unless `len > 4` and the last 4 chars are `.cub`. |
| `count_lines` | `int count_lines(const char *path)` | Opens, loops `get_next_line`, frees each, returns count; -1 if open fails, -2 if the first read fails (directory). |
| `read_lines` | `char **read_lines(const char *path, int n)` | `ft_calloc(n + 1)`, opens, reads `n` lines with gnl, strips the trailing `\n` from each (`strip_newline`), closes. NULL on malloc failure. |
| `strip_newline` | `static void strip_newline(char *line)` | Replaces the first `\n` with `\0`. |
| `file_is_blank` | `int file_is_blank(char **lines)` | 1 if every line is blank (→ F6). |

Two passes (count, then read) avoid writing a realloc; both passes are O(n) and the
file is tiny. gnl is called until it returns NULL in both passes so its static buffer is
left clean for the next open.

### `src/parse_line.c` — classifying lines

| Function | Signature | Does |
|---|---|---|
| `is_blank` | `int is_blank(const char *s)` | 1 if `s` has only `' '`/`'\t'` or is empty. |
| `skip_ws` | `const char *skip_ws(const char *s)` | Returns pointer past leading spaces/tabs. |
| `get_identifier` | `int get_identifier(const char *s)` | After `skip_ws`, compares against the table `{"NO","SO","WE","EA","F","C"}`; the match must be followed by space, tab or `\0`. Returns 0..5 or -1. |
| `is_map_line` | `int is_map_line(const char *s)` | After `skip_ws`, first char is one of `0 1 N S E W` **and** `get_identifier(s) == -1`. |
| `tabs_to_spaces` | `void tabs_to_spaces(char *s)` | In-place `\t` → `' '` so `ft_split(s, ' ')` sees one separator kind. |

### `src/parse_element.c` — one element line

| Function | Signature | Does |
|---|---|---|
| `parse_element` | `const char *parse_element(t_map *m, char *line)` | `tabs_to_spaces`, `ft_split(line, ' ')`; needs exactly 2 tokens (else I3/I4); `id = get_identifier(tok[0])` (I1); dispatches to `set_texture` for 0..3, `set_color` for 4..5; frees tokens; returns the callee's error or NULL. |
| `set_texture` | `const char *set_texture(t_map *m, int side, const char *path)` | I2 if `tex_path[side]` already set; T3 extension; `check_readable(path)` for T1/T2/T4; `ft_strdup` into `tex_path[side]`. |
| `check_readable` | `static const char *check_readable(const char *path)` | `open` (T1); `read(fd, &c, 1)`: -1 → T2, 0 → T4; close; NULL on success. |
| `set_color` | `const char *set_color(t_map *m, int which, const char *tok)` | I2 via `floor_set`/`ceil_set`; `parse_rgb(tok, &rgb)` (C-cases); stores and sets the flag. |

### `src/parse_rgb.c` — strict RGB

| Function | Signature | Does |
|---|---|---|
| `parse_channel` | `static int parse_channel(const char *s, int *i)` | Reads 1–3 digits at `s[*i]`, advances `*i`, returns value 0–255, or -1 (no digit / 4+ digits), or -2 (> 255). |
| `parse_rgb` | `const char *parse_rgb(const char *s, int *out)` | Three `parse_channel` calls separated by exactly one `,`, then `\0`. Packs `r<<16 | g<<8 | b`. Returns C-message or NULL. |

Whitespace decision: **no spaces anywhere inside the triplet**. The token has already
been produced by `ft_split`, so a space would have split it (→ I4 "too many values").
Justification: the subject's own example is `F 220,100,0`; accepting `220, 100,0` would
force a second tokenizer and makes `220 ,100,0` vs `2 20,100,0` ambiguous. A strict
rule with an explicit message is easy to defend.

### `src/parse_map_start.c` — where the map begins, collecting it

| Function | Signature | Does |
|---|---|---|
| `find_map_start` | `int find_map_start(char **lines, t_map *m, const char **err)` | Walks lines: blank → skip; element → `parse_element`; map line → return its index; anything else → I1. Returns -1 and sets `*err` on error, or when EOF is reached without a map line (O2). |
| `collect_map` | `const char *collect_map(t_map *m, char **lines, int start)` | From `start` to EOF: element line → O1; non-map non-blank → O4; else copy. Trailing blank lines are dropped; interior blank lines are kept for Phase 2 to reject. Result into `m->grid` (`ft_calloc` + `ft_strdup`). |
| `check_all_set` | `const char *check_all_set(t_map *m)` | I6 in fixed order NO,SO,WE,EA,F,C; then O2 if `grid == NULL`. |

### `src/parse.c` — entry point

| Function | Signature | Does |
|---|---|---|
| `parse_cub` | `void parse_cub(t_game *g, const char *path)` | `check_extension` → `count_lines` → `read_lines` → `file_is_blank` → `find_map_start` → `collect_map` → `check_all_set`. On any error: `ft_free_tab(lines)` then `exit_error`. On success frees `lines` and returns. |

`main.c` change: `if (argc != 2) exit_error(..., "usage: ./cub3D <map.cub>")`, then
`parse_cub(&game, argv[1])` **before** `init_window` (no X connection needed to reject a
bad file; also keeps valgrind output clean for parser tests).

File count check against the Norm: 5 functions max per file — each file above has ≤ 5.

---

## 4. Pseudocode for the two tricky functions

### 4a. `parse_rgb` (strict)

```
parse_channel(s, i):
    if not isdigit(s[i]): return -1
    val = 0, digits = 0
    while isdigit(s[i]):
        val = val * 10 + (s[i] - '0')
        digits++
        if digits > 3: return -1          # "0255" rejected, "007" accepted
        i++
    if val > 255: return -2
    return val

parse_rgb(s, out):
    i = 0
    for c in 0..2:                        # three channels (write as while loop)
        v[c] = parse_channel(s, &i)
        if v[c] == -2: return "color channel out of range (0-255)"
        if v[c] <  0:  return "invalid color value"
        if c < 2:
            if s[i] != ',': return "invalid color value"   # catches "255 0 0", "255"
            i++                                             # ",," → next channel has no digit → -1
    if s[i] != '\0': return "invalid color value"           # catches "255,0,0," and "1,2,3,4"
    *out = (v[0] << 16) | (v[1] << 8) | v[2]
    return NULL
```

Inputs and outcomes:

| input | result |
|---|---|
| `220,100,0` | ok 0xDC6400 |
| `255,,0` | second `parse_channel` sees `,` → -1 → invalid |
| `255,0,0,` | after third channel `s[i] == ','` ≠ `\0` → invalid |
| `a,b,c` | first channel -1 → invalid |
| `-1,0,0` | `-` is not a digit → invalid |
| `256,0,0` | -2 → out of range |
| `0255,0,0` | 4 digits → -1 → invalid |

### 4b. `find_map_start` + `collect_map` (where the map begins, element after map)

```
find_map_start(lines, map, err):
    i = 0
    while lines[i] != NULL:
        if is_blank(lines[i]):            i++, continue
        if get_identifier(lines[i]) >= 0:
            *err = parse_element(map, lines[i])
            if *err: return -1
            i++, continue
        if is_map_line(lines[i]):         return i          # first map-like line = map start
        *err = "unknown identifier"       # e.g. "hello", "no ./a.xpm"
        return -1
    *err = "missing map"
    return -1

collect_map(map, lines, start):
    end = last index whose line is not blank   # drop trailing blank lines only
    if end < start: return "missing map"
    grid = calloc(end - start + 2)
    for i in start..end:
        if get_identifier(lines[i]) >= 0:      # "NO ./x.xpm" sitting inside/after the map
            free(grid); return "element found after map start"
        if not is_blank(lines[i]) and not is_map_line(lines[i]):
            free(grid); return "invalid line in map section"
        grid[i - start] = strdup(lines[i])     # blank interior lines kept; Phase 2 rejects them
    map->grid = grid
    return NULL
```

Why "element after map" cannot be confused with a map line: `is_map_line` is defined as
"starts with one of 0 1 N S E W **and** `get_identifier` fails". `NO ./a.xpm` matches
the identifier table (`NO` followed by a space), so it is an element wherever it
appears; after `start` that is always an error. A genuine map line such as `N001` is
not an identifier (`N0` ≠ `NO`, and `NO` would need a following space), so it stays a
map line. The one ambiguous shape, `NO` alone on a line inside the map, is treated as an
element (identifier followed by `\0`) and rejected — a map row consisting of the
letters N and O is invalid anyway.

Blank lines: before the map they are separators and skipped; after the first map line
they are collected so that Phase 2 can report "empty line inside map" with a line
number; blank lines after the last map row are dropped, which the subject permits.

---

## 5. Test scene files

All textures referenced below exist as tiny placeholder XPMs in `textures/`.
Files are under `maps/invalid/` and `maps/valid/`.

Must FAIL
1. `maps/invalid/dup_identifier.cub` — `NO` given twice → `duplicate identifier: NO`
2. `maps/invalid/bad_rgb.cub` — `F 255,,0` → `invalid color value: 255,,0`
3. `maps/invalid/missing_texture.cub` — `WE ./textures/nope.xpm` → `cannot open texture`
4. `maps/invalid/element_after_map.cub` — `C 0,0,0` placed after map rows → `element found after map start: C`
5. `maps/invalid/missing_element.cub` — no `EA` line → `missing element: EA`

Must PASS
6. `maps/valid/basic.cub` — canonical layout, one space between tokens
7. `maps/valid/spaced.cub` — elements in shuffled order, several blank lines between
   them, multiple spaces/tabs inside element lines, blank lines after the map

Extra shell checks that need no file:
```bash
./cub3D                       # F1 usage
./cub3D maps/valid/basic.cu   # F2 extension
./cub3D maps                  # F5 directory
./cub3D maps/none.cub         # F4 cannot open
valgrind --leak-check=full ./cub3D maps/invalid/bad_rgb.cub   # must be leak-free
```
