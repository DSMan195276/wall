
#include <math.h>
#include <stdio.h>
#include <string.h>

#include "gl_math.h"
#include "render.h"

void render_element_rotate(struct render_element *element)
{
    float section = M_PI / (160.f);
    struct mat4 rot;

    mat4_make_rotation(&rot, section, ROT_Y);
    mat4_mult(&element->model, &element->model, &rot);
}

