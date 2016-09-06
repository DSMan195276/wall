#ifndef INCLUDE_CUBE_H
#define INCLUDE_CUBE_H

#include "gl_math.h"

void render_cube(struct tri *tri, struct cube *cube);
void render_cube_no_top_or_bottom(struct tri *tri, struct cube *cube);

#endif
