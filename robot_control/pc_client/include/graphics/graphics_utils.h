#ifndef GRAPHICS_UTILS_H
# define GRAPHICS_UTILS_H

# include <X11/keysym.h>
# include "mlx.h"

# include "shared_types.h"
# include "circular_buffer.h"

# define FRAME_WIDTH 1200
# define FRAME_HEIGHT 700
# define PADDING_X 60
# define PADDING_Y 20
# define PLANE_WIDTH (FRAME_WIDTH - 2 * PADDING_X)
# define PLANE_HEIGHT (FRAME_HEIGHT - 2 * PADDING_Y)
# define ORIGIN_X PADDING_X
# define ORIGIN_Y FRAME_HEIGHT / 2
# define RECT_X (FRAME_WIDTH - 2 * PADDING_X) / 10
# define RECT_Y (FRAME_HEIGHT - 2 * PADDING_Y) / 10

typedef struct	s_img {
	void	*img;
	char	*pixels;
	int		bits_per_pixel;
	int		line_length;
	int		endian;
} t_img;

typedef struct s_windata{
    void *mlx;
    void *win;
    t_img img;
    t_info axis_info;
} t_windata;

uint32_t	create_trgb(uint8_t t, uint8_t r, uint8_t g, uint8_t b);
void		my_mlx_pixel_put(t_img *data, int x, int y, int color);
void		window_init(t_windata *data, const char *win_name);
void		new_image_init(t_windata *data);
void		draw_info(void *mlx, void *win, t_info *axis_info);
void		draw_data(t_img *img, CircularBuffer *cbuf, t_info *axis_info);
void		plot_data(t_windata *windata, CircularBuffer *cbuf, t_info *axis_info);

int			key_handler(int keysym, t_info *axis_info);

#endif