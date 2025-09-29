#ifndef EVENT_UTILS_H
# define EVENT_UTILS_H

# include <X11/keysym.h>
# include "mlx.h"

# include "shared_types.h"

int	key_handler(int keysym, t_info *axis_info);

#endif