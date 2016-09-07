
#include <stdlib.h>
#include <math.h>

#include "container_of.h"
#include "gl_math.h"

void cone_render(struct tri **tri, int *tri_count, float base_rad, float top_rad, float ymin, float ymax, int steps)
{
    int i;
    float dtheta = (M_PI * 2) / steps;
    float theta = 0;

    *tri = malloc(sizeof(**tri) * steps * 2);
    *tri_count = steps * 2;

    for (i = 0; i < steps * 2; i += 2) {
        (*tri)[i + 0].p1 = (struct vec3) { .x = cosf(theta) * top_rad,           .y = ymax, .z = sinf(theta) * top_rad };
        (*tri)[i + 0].p2 = (struct vec3) { .x = cosf(theta + dtheta) * top_rad,  .y = ymax, .z = sinf(theta + dtheta) * top_rad };
        (*tri)[i + 0].p3 = (struct vec3) { .x = cos(theta) * base_rad,           .y = ymin, .z = sinf(theta) * base_rad };

        (*tri)[i + 1].p1 = (struct vec3) { .x = cosf(theta + dtheta) * top_rad,  .y = ymax, .z = sinf(theta + dtheta) * top_rad };
        (*tri)[i + 1].p2 = (struct vec3) { .x = cosf(theta) * base_rad,          .y = ymin, .z = sinf(theta) * base_rad};
        (*tri)[i + 1].p3 = (struct vec3) { .x = cosf(theta + dtheta) * base_rad, .y = ymin, .z = sinf(theta + dtheta) * base_rad };

        theta += dtheta;
    }
}

