#include "graphics_utils.h"

// takes trgb values and condensate them into a single 32 bit register
uint32_t create_trgb(uint8_t t, uint8_t r, uint8_t g, uint8_t b) {
	return (t << 24 | r << 16 | g << 8 | b);
}

// this fun will just fill a pixel in the pixels buffer 
void	my_mlx_pixel_put(t_img *img, int x, int y, int color) {
	char *dst;

	dst = img->pixels + (y * img->line_length + x * (img->bits_per_pixel / 8));
	*(unsigned int*)dst = color;
}

void draw_grid(t_img *img) {
	// draw horizontal lines
	for (int i = 20; i < HEIGHT - 20; i += 50) {
		for (int j = 20; j < WIDTH - 20; j++) {
			my_mlx_pixel_put(img, j, i, 0x0FFFFFFF);
		}
	}

	// draw vertical lines
	for (int i = 20; i < HEIGHT - 20; i++) {
		for (int j = 20; j < WIDTH - 20; j += 100) {
			my_mlx_pixel_put(img, j, i, 0x0FFFFFFF);
		}
	}
	
}

void draw_data(int **sample_data, t_img *img) {
	draw_grid(img);
}