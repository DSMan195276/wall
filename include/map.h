#ifndef INCLUDE_MAP_H
#define INCLUDE_MAP_H

#include "gl_math.h"

struct map {
    int width, height;
    int *walls; /* width * height */
};

void map_render(struct map *map, struct tri **tri_map, int *tri_count);

#endif
