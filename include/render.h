#ifndef INCLUDE_RENDER_H
#define INCLUDE_RENDER_H

#include "gl_math.h"
#include "map.h"

void render_cube(struct tri *tri, struct cube *cube);
void render_main_loop(struct map *map);

int renderer_start(void);
int renderer_end(void);

#endif
