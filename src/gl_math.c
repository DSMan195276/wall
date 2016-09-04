
#include <math.h>
#include <stdio.h>
#include <string.h>

#include "gl_math.h"

void vec3_sub(struct vec3 *out, const struct vec3 *a, const struct vec3 *b)
{
    *out = (struct vec3) { .x = a->x - b->x, .y = a->y - b->y, .z = a->z - b->z };
}

void vec3_add(struct vec3 *out, const struct vec3 *a, const struct vec3 *b)
{
    *out = (struct vec3) { .x = a->x + b->x, .y = a->y + b->y, .z = a->z + b->z };
}

void vec3_mul_scaler(struct vec3 *out, const struct vec3 *a, float scaler)
{
    *out = (struct vec3) { .x = a->x * scaler, .y = a->y * scaler, .z = a->z * scaler };
}

float vec3_length2(struct vec3 *vec)
{
    return vec->x * vec->x + vec->y * vec->y + vec->z * vec->z;
}

float vec3_length(struct vec3 *vec)
{
    return sqrtf(vec3_length2(vec));
}

void vec3_normalize(struct vec3 *vec)
{
    float norm = vec3_length(vec);
    if (norm == 0)
        return ;

    vec->x = vec->x / norm;
    vec->y = vec->y / norm;
    vec->z = vec->z / norm;
}

void vec3_cross(struct vec3 *out, const struct vec3 *a, const struct vec3 *b)
{
    float x = a->y * b->z - a->z * b->y;
    float y = a->z * b->x - a->x * b->z;
    float z = a->x * b->y - a->y * b->x;
    *out = (struct vec3) { .x = x, .y = y, .z = z };
}

void mat4_make_translation(struct mat4 *mat, const struct vec3 *v)
{
    *mat = (struct mat4) {
        .m = {
            { 1, 0, 0, v->x },
            { 0, 1, 0, v->y },
            { 0, 0, 1, v->z },
            { 0, 0, 0, 1 },
        },
    };
}

void mat4_make_scale(struct mat4 *mat, const struct vec3 *v)
{
    *mat = (struct mat4) {
        .m = {
            { v->x, 0,    0,    0 },
            { 0,    v->y, 0,    0 },
            { 0,    0,    v->z, 0 },
            { 0,    0,    0,    1 },
        },
    };
}

static void mat4_rot_x(struct mat4 *mat, float c, float s)
{
    *mat = (struct mat4) {
        .m = {
            { 1, 0, 0, 0 },
            { 0, c, -s, 0 },
            { 0, s, c, 0 },
            { 0, 0, 0, 1 },
        },
    };
}

static void mat4_rot_y(struct mat4 *mat, float c, float s)
{
    *mat = (struct mat4) {
        .m = {
            { c, 0, s, 0 },
            { 0, 1, 0, 0 },
            { -s, 0, c, 0 },
            { 0, 0, 0, 1 },
        },
    };
}

static void mat4_rot_z(struct mat4 *mat, float c, float s)
{
    *mat = (struct mat4) {
        .m = {
            { c, -s, 0, 0 },
            { s, c, 0, 0 },
            { 0, 0, 1, 0 },
            { 0, 0, 0, 1 },
        },
    };
}

void mat4_make_rotation(struct mat4 *mat, float theta, enum rot rot)
{
    float c = cosf(theta);
    float s = sinf(theta);

    switch (rot) {
    case ROT_X:
        mat4_rot_x(mat, c, s);
        return ;

    case ROT_Y:
        mat4_rot_y(mat, c, s);
        return ;

    case ROT_Z:
        mat4_rot_z(mat, c, s);
        return ;
    }

    return ;
}

void mat4_mult(struct mat4 *out, const struct mat4 *mat, const struct mat4 *mat2)
{
    const char size = 4;
    const float (*a)[4] = mat->m, (*b)[4] = mat2->m;
    int i, k, j;

    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            float sum = 0;
            for (k = 0; k < size; k++)
                sum += a[i][k] * b[k][j];

            out->m[i][j] = sum;
        }
    }
}

void mat4_mult_vec4(struct vec4 *out, const struct mat4 *mat, const struct vec4 *vec)
{
    const char height = 4, width = 1;
    const float (*a)[4] = mat->m, (*b)[1] = (float (*)[1])vec->m;
    int i, j, k;

    for (i = 0; i < height; i++) {
        for (j = 0; j < width; j++) {
            float sum = 0;
            for (k = 0; k < height; k++)
                sum += a[i][j] * b[k][j];

            out->m[i * height + j] = sum;
        }
    }
}

void mat4_look_at(struct mat4 *out, const struct vec3 *cam, const struct vec3 *target, const struct vec3 *up)
{
    struct mat4 tmp, tmp2;
    struct vec3 d, r, u, up_p;

    up_p = *up;
    vec3_normalize(&up_p);

    vec3_sub(&d, target, cam);
    vec3_normalize(&d);

    vec3_cross(&r, &d, &up_p);
    vec3_normalize(&r);

    vec3_cross(&u, &r, &d);
    vec3_normalize(&u);

    tmp2 = (struct mat4) {
        .m = {
            { r.x, r.y, r.z, 0 },
            { u.x, u.y, u.z, 0 },
            { -d.x, -d.y, -d.z, 0 },
            { 0, 0, 0, 1 },
        },
    };

    mat4_make_translation(&tmp, &(struct vec3) { .x = -cam->x, .y = -cam->y, .z = -cam->z });

    mat4_mult(out, &tmp2, &tmp);
}

void mat4_make_perspective(struct mat4 *out, float fov, float aspect, float near, float far)
{
    float f = 1.0 / tanf(fov * (2.f * M_PI / 360.f) / 2.0f);
    float denom = near - far;

    float a = f / aspect;
    float b = f;
    float c = (far + near) / denom;
    float d = (2.0f * far * near) / denom;

    *out = (struct mat4) {
        .m = {
            { a, 0, 0, 0 },
            { 0, b, 0, 0 },
            { 0, 0, c, d },
            { 0, 0, -1, 0 },
        },
    };
}

void print_vec3(struct vec3 *vec)
{
    printf("[%5f %5f %5f]\n", vec->x, vec->y, vec->z);
}

void print_mat4(struct mat4 *mat)
{
    printf("[\n");
    printf("  [%5f %5f %5f %5f]\n", mat->m[0][0], mat->m[0][1], mat->m[0][2], mat->m[0][3]);
    printf("  [%5f %5f %5f %5f]\n", mat->m[1][0], mat->m[1][1], mat->m[1][2], mat->m[1][3]);
    printf("  [%5f %5f %5f %5f]\n", mat->m[2][0], mat->m[2][1], mat->m[2][2], mat->m[2][3]);
    printf("  [%5f %5f %5f %5f]\n", mat->m[3][0], mat->m[3][1], mat->m[3][2], mat->m[3][3]);
    printf("]\n");
}

void print_cube(struct cube *cube)
{
    printf("Cube: (%f, %f, %f) -> (%f, %f, %f)\n",
            cube->tlf.x, cube->tlf.y, cube->tlf.z,
            cube->brb.x, cube->brb.y, cube->brb.z);
}

