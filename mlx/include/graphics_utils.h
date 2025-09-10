#include <X11/keysym.h>
#include <stdint.h>

#include "mlx.h"

#define WIDTH 700
#define HEIGHT 500

typedef struct	s_img {
	void	*img;
	char	*pixels;
	int		bits_per_pixel;
	int		line_length;
	int		endian;
} t_img;

uint32_t	create_trgb(uint8_t t, uint8_t r, uint8_t g, uint8_t b);
void		my_mlx_pixel_put(t_img *data, int x, int y, int color);
void		draw_data(int **sample_data, t_img *img);