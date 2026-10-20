# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: esakgul <esakgul@student.42istanbul.com    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/10/20 15:00:00 by esakgul           #+#    #+#              #
#    Updated: 2026/10/20 15:00:00 by esakgul          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME        := cub3D

CC          := cc

CFLAGS      := -Wall -Wextra -Werror -MMD -MP
INCS        := -I inc -I libft -I minilibx-linux

LIBFT_DIR   := libft
LIBFT       := $(LIBFT_DIR)/libft.a
MLX_DIR     := minilibx-linux
MLX         := $(MLX_DIR)/libmlx.a

LDFLAGS     := -L$(LIBFT_DIR) -lft -L$(MLX_DIR) -lmlx -lXext -lX11 -lm

SRC_DIR     := src
OBJ_DIR     := obj
BOBJ_DIR    := obj_bonus

SRCS        := main.c \
               hooks.c \
               cleanup.c


BONUS_ONLY  :=
BONUS_SRCS  := $(SRCS) $(BONUS_ONLY)

OBJS        := $(addprefix $(OBJ_DIR)/,  $(SRCS:.c=.o))
BOBJS       := $(addprefix $(BOBJ_DIR)/, $(BONUS_SRCS:.c=.o))
DEPS        := $(OBJS:.o=.d) $(BOBJS:.o=.d)

LIBFT_SRCS  := $(wildcard $(LIBFT_DIR)/*.c) $(wildcard $(LIBFT_DIR)/*.h)

all: $(NAME)

$(NAME): $(LIBFT) $(MLX) $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) -o $@

bonus: .bonus

.bonus: $(LIBFT) $(MLX) $(BOBJS)
	$(CC) $(CFLAGS) $(BOBJS) $(LDFLAGS) -o $(NAME)
	@touch $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCS) -c $< -o $@

$(BOBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -D BONUS $(INCS) -c $< -o $@

$(LIBFT): $(LIBFT_SRCS)
	$(MAKE) -C $(LIBFT_DIR)

$(MLX):
	$(MAKE) -C $(MLX_DIR)

clean:
	rm -rf $(OBJ_DIR) $(BOBJ_DIR) .bonus
	$(MAKE) -C $(LIBFT_DIR) clean

fclean: clean
	rm -f $(NAME)
	$(MAKE) -C $(LIBFT_DIR) fclean
	$(MAKE) -C $(MLX_DIR) clean

re: fclean all

-include $(DEPS)

.PHONY: all bonus clean fclean re
