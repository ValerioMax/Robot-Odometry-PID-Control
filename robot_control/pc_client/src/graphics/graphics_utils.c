#include "graphics_utils.h"

// takes trgb values and condensate them into a single 32 bit register
uint32_t create_trgb(uint8_t t, uint8_t r, uint8_t g, uint8_t b) {
	return (t << 24 | r << 16 | g << 8 | b);
}

// this fun will just fill a pixel in the pixels buffer 
void my_mlx_pixel_put(t_img *img, int x, int y, int color) {
	char *dst;

	dst = img->pixels + (y * img->line_length + x * (img->bits_per_pixel / 8));
	*(unsigned int*)dst = color;
}

void init_window(t_windata *data) {
    // initialize graphical system, identified by void *mlx
    data->mlx = mlx_init();

    // initialize window
	data->win = mlx_new_window(data->mlx, FRAME_WIDTH, FRAME_HEIGHT, "prova");

    // initiate image and associate it with the window
    data->img.img = mlx_new_image(data->mlx, FRAME_WIDTH, FRAME_HEIGHT);

    // img.pixels is the buffer containg all the pixels data (in RGBA every pixel has 4 bytes)
    // this fun sets parameters of this buffer
	data->img.pixels = mlx_get_data_addr(data->img.img, &data->img.bits_per_pixel, &data->img.line_length, &data->img.endian);
}

// draws numbers on the axis (does a linspace in proportion with plane pixels dimentions)
void draw_info(void *mlx, void *win, t_info *axis_info) {
	char str[10];
	int norm_time;
	int norm_value;

	// increments for numbers in the axis
	int x_norm_increment = (int) ((axis_info->time_range / PLANE_WIDTH) * RECT_X);
	int y_norm_increment = (int) (((float) axis_info->value_max / (PLANE_HEIGHT / 2)) * RECT_Y);

	// values on y-axis (positive)
	norm_value = 0;
	for (int i = ORIGIN_Y; i >= PADDING_Y; i -= RECT_Y) {
		sprintf(str, "%d", norm_value); // integer to string into buffer str
		mlx_string_put(mlx, win, ORIGIN_X - PADDING_X / 2, i, 0x0FFFFFFF, str);
		norm_value += y_norm_increment;
	}
	// values on y-axis (negative)
	norm_value = -y_norm_increment;
	for (int i = ORIGIN_Y + RECT_Y; i < PLANE_HEIGHT + PADDING_Y; i += RECT_Y) {
		sprintf(str, "%d", norm_value); // integer to string into buffer str
		mlx_string_put(mlx, win, ORIGIN_X - PADDING_X / 2, i, 0x0FFFFFFF, str);
		norm_value -= y_norm_increment;
	}
	// values on x-axis
	norm_time = axis_info->time_min;
	for (int i = PADDING_X + 5; i <= PLANE_WIDTH + PADDING_X; i += RECT_X) {
		sprintf(str, "%d", norm_time); // integer to string into buffer str
		mlx_string_put(mlx, win, i, ORIGIN_Y + 15, 0x0FFFFFFF, str);
		norm_time += x_norm_increment;
	}		
}

void draw_grid(t_img *img) {
	// draw horizontal lines
	for (int i = PADDING_Y; i < PLANE_HEIGHT + PADDING_Y; i += RECT_Y)
		for (int j = PADDING_X; j < PLANE_WIDTH + PADDING_X; j++)
			my_mlx_pixel_put(img, j, i, 0x0FFFFFFF);

	// draw vertical lines
	for (int i = PADDING_Y; i < PLANE_HEIGHT + PADDING_Y; i++)
		for (int j = PADDING_X; j < PLANE_WIDTH + PADDING_X; j += RECT_X)
			my_mlx_pixel_put(img, j, i, 0x0FFFFFFF);
}

double map(double value, double min1, double max1, double min2, double max2) {
	double norm_value = (value - min1) / (max1 - min1);
	return min2 + (max2 - min2) * norm_value;
}

// interpolate previous point with current to make a continous curve
void interpolate_and_draw(t_img *img, int prev_x, int prev_y, int curr_x, int curr_y) {
	while (prev_x < curr_x) {
		my_mlx_pixel_put(img, prev_x, prev_y, 0x0FFFFF00);
		prev_x++;
	}

	// True interpolation but it difficult (gotta compute slope, ecc.)
	/*
	if (curr_y > prev_y)
		for (int i = prev_y; i < curr_y; i++) {
			my_mlx_pixel_put(img, prev_x + i, ORIGIN_Y - i, 0x0FFFFF00);
		}
	else if (curr_y < prev_y)
		for (int i = curr_y; i < prev_y; i++) {
			my_mlx_pixel_put(img, prev_x + i, ORIGIN_Y - i, 0x0FFFFF00);
		}
	else
		for (int i = prev_x; i < curr_x; i++) {
			my_mlx_pixel_put(img, ORIGIN_X + i, ORIGIN_Y - prev_y, 0x0FFFFF00);
		}
	*/
}

void draw_data(t_sample *sample_data[], t_img *img, t_info *axis_info) {
	draw_grid(img);
	
	if (!sample_data)
		return ;
	
	if (!axis_info->time_range)
		return ;

	//int curr_max = HEIGHT / 10;
	int prev_time = 0;
	int prev_value = 0;

	for (int i = 0; i < NUM_SAMPLES; i++) {
		t_sample *sample = sample_data[i];
	
		if (sample) {
			// normilize timestamp respect to current time_range
			//int norm_time = (int) ((sample->timestamp / axis_info->time_range) * PLANE_WIDTH);
			// normalize value respect to a maximum fixed by myself
			//int norm_value = (int) (((float) sample->value / axis_info->value_max) * (PLANE_HEIGHT / 2));

			int mapped_time = (int) map(sample->timestamp, axis_info->time_min, axis_info->time_max, ORIGIN_X, PLANE_WIDTH + ORIGIN_X);
			int mapped_value = (int) map(sample->value, -axis_info->value_max, axis_info->value_max, PLANE_HEIGHT + PADDING_Y, PADDING_Y);

			interpolate_and_draw(img, prev_time, prev_value, mapped_time, mapped_value);

			prev_time = mapped_time;
			prev_value = mapped_value;

			// normalize value respect to a maximum value got so far
			//int norm_value = (sample->value / curr_max) * HEIGHT;
			//if (sample->value > curr_max)
			//	curr_max = sample->value;

			//my_mlx_pixel_put(img, mapped_time, mapped_value, 0x0FFFFF00);
		}
	}
}