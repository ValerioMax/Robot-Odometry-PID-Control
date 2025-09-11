NAME = prova

CC = gcc

INCLUDES = -I include

SRCS_DIR = src

SRCS =	src/*

MLX_FLAGS = -Llib/. -lmlx_Linux -lX11 -lXext -lm #-lz -lXrender

MLX_LIB = lib/libmlx_Linux.a

all : $(NAME)

$(NAME) :	$(SRCS)
			$(CC) $(SRCS) -o $(NAME) $(INCLUDES) $(MLX_FLAGS)

clean :
	rm $(NAME)
