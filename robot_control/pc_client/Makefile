NAME = prova

CC = gcc

# includes
INCLUDE_DIR = include

INCLUDE_THREAD_READ_DIR = $(INCLUDE_DIR)/thread_read
INCLUDE_THREAD_WRITE_DIR = $(INCLUDE_DIR)/thread_write
INCLUDE_GRAPHICS = $(INCLUDE_DIR)/graphics

INCLUDES = -I $(INCLUDE_THREAD_READ_DIR) -I $(INCLUDE_THREAD_WRITE_DIR) -I $(INCLUDE_GRAPHICS)

# sources
SRC_DIR = src

THREAD_READ_SRCS_DIR =	$(SRC_DIR)/thread_read
THREAD_WRITE_SRCS_DIR =	$(SRC_DIR)/thread_write
GRAPHICS_SRCS_DIR = $(SRC_DIR)/graphics

SRCS = $(THREAD_READ_SRCS_DIR)/*.c $(THREAD_WRITE_SRCS_DIR)/*.c $(GRAPHICS_SRCS_DIR)/*.c $(SRC_DIR)/main.c

# libraries
MLX_FLAGS = -Llib/. -lmlx_Linux -lX11 -lXext -lm #-lz -lXrender

MLX_LIB = lib/libmlx_Linux.a




all : $(NAME)

$(NAME) :	$(SRCS)
			$(CC) $(SRCS) -o $(NAME) $(INCLUDES) $(MLX_FLAGS)

clean :
	rm $(NAME)