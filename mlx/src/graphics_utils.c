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
	for (int i = 20; i < HEIGHT - 20; i += 50)
		for (int j = 20; j < WIDTH - 20; j++)
			my_mlx_pixel_put(img, j, i, 0x0FFFFFFF);

	// draw vertical lines
	for (int i = 20; i < HEIGHT - 20; i++)
		for (int j = 20; j < WIDTH - 20; j += 100)
			my_mlx_pixel_put(img, j, i, 0x0FFFFFFF);
}

void draw_data(t_sample *sample_data[], t_img *img) {
	draw_grid(img);
	
	if (!sample_data)
		return ;

	if (!sample_data[0] || !sample_data[NUM_SAMPLES - 1])
		return ;
	
	// get time_min and time_max to get y-axis range
	double time_min = (double) sample_data[0]->timestamp;
	double time_max = (double) sample_data[NUM_SAMPLES - 1]->timestamp;
	double time_range = fabs(time_max - time_min);
	//printf("%lf %lf\n", time_min, time_max);
	
	if (!time_range)
		return ;

	//int max_value = HEIGHT / 10;

	for (int i = 0; i < NUM_SAMPLES; i++) {
		t_sample *sample = sample_data[i];
	
		if (sample) {
			// normalize values before drawing them
			int norm_time = (int)((sample->timestamp / time_range) * WIDTH);
			//int norm_value = (sample->value / max_value) * HEIGHT;
			//if (sample->value > max_value)
			//	max_value = sample->value;

			my_mlx_pixel_put(img, norm_time, sample->value, 0x0FFFFF00);
			//my_mlx_pixel_put(img, i, i, 0x0FFFFF00);
		}
	}
}