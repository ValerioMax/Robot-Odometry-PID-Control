#include "event_utils.h"

int	key_handler(int keysym, t_info *axis_info) {
	if (keysym == XK_Escape)
		exit(1);
	if (keysym == XK_z) {
		axis_info->value_max += 100;
	}
	if (keysym == XK_x) {
		axis_info->value_max -= 100;
	}

	return 0;
}