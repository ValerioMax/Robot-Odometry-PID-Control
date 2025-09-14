#ifndef GRAPHICS_UTILS_H
# define GRAPHICS_UTILS_H
# include <X11/keysym.h>
# include <stdint.h>
# include <stdlib.h>
# include "mlx.h"

# include "shared_types.h"

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
void		init_window(t_windata *data);
void		draw_data(t_sample *sample_data[], t_img *img, t_info *axis_info);
void		draw_info(void *mlx, void *win, t_info *axis_info);

void new_image_init(t_windata *data);
void		plot_data(t_sample *sample_data[], t_windata *windata, t_info *axis_info);


#endif