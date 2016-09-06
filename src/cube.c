
#include "gl_math.h"

/* tri is an array of 12 triangles */
void render_cube(struct tri *tri, struct cube *cube)
{
    struct vec3 tlf, tlb, trf, trb;
    struct vec3 blf, blb, brf, brb;

    tlf = cube->tlf;
    brb = cube->brb;

    tlb = (struct vec3) { .x = tlf.x, .y = tlf.y, .z = brb.z };
    trf = (struct vec3) { .x = brb.x, .y = tlf.y, .z = tlf.z };
    trb = (struct vec3) { .x = brb.x, .y = tlf.y, .z = brb.z };

    blb = (struct vec3) { .x = tlf.x, .y = brb.y, .z = brb.z };
    brf = (struct vec3) { .x = brb.x, .y = brb.y, .z = tlf.z };
    blf = (struct vec3) { .x = tlf.x, .y = brb.y, .z = tlf.z };

    tri[0] = (struct tri) { .p1 = tlf, .p2 = trf, .p3 = blf };
    tri[1] = (struct tri) { .p1 = brf, .p2 = trf, .p3 = blf };

    tri[2] = (struct tri) { .p1 = brf, .p2 = brb, .p3 = trf };
    tri[3] = (struct tri) { .p1 = trf, .p2 = trb, .p3 = brb };

    tri[4] = (struct tri) { .p1 = brb, .p2 = blb, .p3 = trb };
    tri[5] = (struct tri) { .p1 = tlb, .p2 = trb, .p3 = blb };

    tri[6] = (struct tri) { .p1 = tlf, .p2 = tlb, .p3 = blb };
    tri[7] = (struct tri) { .p1 = blf, .p2 = blb, .p3 = tlf };

    tri[8] = (struct tri) { .p1 = tlf, .p2 = tlb, .p3 = trb };
    tri[9] = (struct tri) { .p1 = tlf, .p2 = trf, .p3 = trb };

    tri[10] = (struct tri) { .p1 = blf, .p2 = blb, .p3 = brb };
    tri[11] = (struct tri) { .p1 = blf, .p2 = brf, .p3 = brb };
}

/* tri is an array of 12 triangles */
void render_cube_no_top_or_bottom(struct tri *tri, struct cube *cube)
{
    struct vec3 tlf, tlb, trf, trb;
    struct vec3 blf, blb, brf, brb;

    tlf = cube->tlf;
    brb = cube->brb;

    tlb = (struct vec3) { .x = tlf.x, .y = tlf.y, .z = brb.z };
    trf = (struct vec3) { .x = brb.x, .y = tlf.y, .z = tlf.z };
    trb = (struct vec3) { .x = brb.x, .y = tlf.y, .z = brb.z };

    blb = (struct vec3) { .x = tlf.x, .y = brb.y, .z = brb.z };
    brf = (struct vec3) { .x = brb.x, .y = brb.y, .z = tlf.z };
    blf = (struct vec3) { .x = tlf.x, .y = brb.y, .z = tlf.z };

    tri[0] = (struct tri) { .p1 = tlf, .p2 = trf, .p3 = blf };
    tri[1] = (struct tri) { .p1 = brf, .p2 = trf, .p3 = blf };

    tri[2] = (struct tri) { .p1 = brf, .p2 = brb, .p3 = trf };
    tri[3] = (struct tri) { .p1 = trf, .p2 = trb, .p3 = brb };

    tri[4] = (struct tri) { .p1 = brb, .p2 = blb, .p3 = trb };
    tri[5] = (struct tri) { .p1 = tlb, .p2 = trb, .p3 = blb };

    tri[6] = (struct tri) { .p1 = tlf, .p2 = tlb, .p3 = blb };
    tri[7] = (struct tri) { .p1 = blf, .p2 = blb, .p3 = tlf };
}

