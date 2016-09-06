#ifndef INCLUDE_ROTATE_H
#define INCLUDE_ROTATE_H

#include "render.h"

struct render_element_rotator {
    struct render_element element;

    struct mat4 orig_model; /* Original model matrix before rotation */
};

void render_element_rotate(struct render_element *);

#endif
