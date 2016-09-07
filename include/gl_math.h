#ifndef INCLUDE_GL_MATH_H
#define INCLUDE_GL_MATH_H

#define __packed __attribute__((packed))

struct vec2 {
    union {
        float m[2];
        struct {
            float x, y;
        };
    };
} __packed;

struct vec3 {
    union {
        float m[3];
        struct {
            float x, y, z;
        };
    };
} __packed;

struct vec4 {
    union {
        float m[4];
        struct {
            float x, y, z, w;
        };
    };
} __packed;

struct tri {
    struct vec3 p1, p2, p3;
} __packed;

struct rec {
    struct vec2 tl, br;
} __packed;

struct cube {
    struct vec3 tlf, brb; /* Top left forward, bottom right back */
} __packed;

struct mat4 {
    float m[4][4];
} __packed;

enum rot {
    ROT_X,
    ROT_Y,
    ROT_Z,
};

void vec3_sub(struct vec3 *out, const struct vec3 *a, const struct vec3 *b);
void vec3_add(struct vec3 *out, const struct vec3 *a, const struct vec3 *b);
void vec3_mul_scaler(struct vec3 *out, const struct vec3 *a, float scaler);
float vec3_length2(struct vec3 *vec);
float vec3_length(struct vec3 *vec);
void vec3_normalize(struct vec3 *vec);
void vec3_cross(struct vec3 *out, const struct vec3 *a, const struct vec3 *b);

void mat4_make_translation(struct mat4 *mat, const struct vec3 *);
void mat4_make_scale(struct mat4 *mat, const struct vec3 *);
void mat4_make_rotation(struct mat4 *mat, float theta, enum rot);
void mat4_mult(struct mat4 *out, const struct mat4 *mat, const struct mat4 *mat2);
void mat4_mult_vec4(struct vec4 *out, const struct mat4 *mat, const struct vec4 *vec);
void mat4_look_at(struct mat4 *out, const struct vec3 *cam, const struct vec3 *target, const struct vec3 *up);
void mat4_make_perspective(struct mat4 *out, float fov, float aspect, float near, float far);

#define MAT4_IDENTITY() \
    { \
        .m = { \
            { 1, 0, 0, 0 }, \
            { 0, 1, 0, 0 }, \
            { 0, 0, 1, 0 }, \
            { 0, 0, 0, 1 }, \
        }, \
    }


static inline void mat4_make_identity(struct mat4 *out)
{
    *out = (struct mat4)MAT4_IDENTITY();
}

#define PRvec3 "[%5f, %5f, %5f]"
#define Pvec3(vec) ((vec)->x), ((vec)->y), ((vec)->z)

void print_vec3(struct vec3 *vec);
void print_mat4(struct mat4 *mat);
void print_cube(struct cube *cube);

#endif
