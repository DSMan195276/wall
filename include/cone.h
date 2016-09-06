#ifndef INCLUDE_CONE_H
#define INCLUDE_CONE_H

#include "gl_math.h"

void cone_render(struct tri **tri, int *tri_count, float base_rad, float top_rad, float ymin, float ymax, int steps);

#endif
