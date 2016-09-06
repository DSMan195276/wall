#ifndef INCLUDE_MAP_H
#define INCLUDE_MAP_H

#include "gl_math.h"
#include "render.h"

struct map {
    int width, height;
    float startx, startz;
    float finishx, finishz;
    int *walls; /* width * height */
};

void map_render(struct map *map, struct tri **tri_map, int *tri_count);
void map_render_floor(struct map *map, struct tri **tri_map, int *tri_count);
void map_render_ceiling(struct map *map, struct tri **tri_map, int *tri_count);

#endif
