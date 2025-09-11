#include "graphics_utils.h"
#include "data_utils.h"

int main() {
    void *mlx;
	void *win;
    t_img img;
    t_sample *sample_data[NUM_SAMPLES];
    t_info axis_info;

    // initialize graphical system, identified by void *mlx
    mlx = mlx_init();

    // initialize window
	win = mlx_new_window(mlx, FRAME_WIDTH, FRAME_HEIGHT, "prova");

    // initiate image and associate it with the window
    img.img = mlx_new_image(mlx, FRAME_WIDTH, FRAME_HEIGHT);

    // img.pixels is the buffer containg all the pixels data (in RGBA every pixel has 4 bytes)
    // this fun sets parameters of this buffer
	img.pixels = mlx_get_data_addr(img.img, &img.bits_per_pixel, &img.line_length, &img.endian);

    sample_data_init(sample_data);
    get_data_from_tsv(sample_data, DATA_FILE_NAME);
    get_axis_info(sample_data, &axis_info);
    draw_data(sample_data, &img, &axis_info);

    // draw the image pixels on the window
	mlx_put_image_to_window(mlx, win, img.img, 0, 0);

    // for reasons strings have to be drawn after the rest
    draw_info(mlx, win, &axis_info);

    // deallocate dynamic data
    free_sample_data(sample_data);

    // initiate window rendering (continously)
	mlx_loop(mlx);
}