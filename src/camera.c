
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "gl_math.h"
#include "camera.h"

void camera_init(struct camera *cam)
{
    memset(cam, 0, sizeof(*cam));
}

void camera_recalc(struct camera *cam)
{
    struct vec3 target_pos;

    cam->direction = (struct vec3) { .x = (cosf(cam->pitch) * sinf(cam->yaw)),
                                     .y = sinf(cam->pitch),
                                     .z = (cosf(cam->pitch) * cosf(cam->yaw)) };
    vec3_normalize(&cam->direction);

    vec3_add(&target_pos, &cam->pos, &cam->direction);

    mat4_look_at(&cam->view, &cam->pos, &target_pos, &cam->up);

    mat4_make_perspective(&cam->proj, cam->fov, cam->aspect, cam->min_depth, cam->max_depth);
}

static void get_usable_direction(struct camera *cam, struct vec3 *out)
{
    if (!cam->flat_movement)
        *out = cam->direction;
    else
        *out = (struct vec3) { .x = cam->direction.x, .y = 0, .z = cam->direction.z };
}

static void camera_move_for_back(struct camera *cam, float seconds, int direction)
{
    struct vec3 move, res, dir;
    get_usable_direction(cam, &dir);

    vec3_mul_scaler(&move, &dir, cam->cam_speed * direction);
    vec3_add(&res, &cam->pos, &move);
    memcpy(&cam->pos, &res, sizeof(cam->pos));
}

void camera_move_forward(struct camera *cam, float seconds)
{
    camera_move_for_back(cam, seconds, 1);
}

void camera_move_backward(struct camera *cam, float seconds)
{
    camera_move_for_back(cam, seconds, -1);
}

static void camera_move_left_right(struct camera *cam, float seconds, int direction)
{
    struct vec3 tmp, move, res, dir;
    get_usable_direction(cam, &dir);

    vec3_cross(&tmp, &dir, &cam->up);
    vec3_normalize(&tmp);

    vec3_mul_scaler(&move, &tmp, cam->cam_speed * direction);
    vec3_add(&res, &cam->pos, &move);
    memcpy(&cam->pos, &res, sizeof(cam->pos));
}

void camera_move_left(struct camera *cam, float seconds)
{
    camera_move_left_right(cam, seconds, -1);
}

void camera_move_right(struct camera *cam, float seconds)
{
    camera_move_left_right(cam, seconds, 1);
}

void camera_rotate_yaw_up(struct camera *cam, float seconds)
{
    cam->yaw += cam->rot_speed;
}

void camera_rotate_yaw_down(struct camera *cam, float seconds)
{
    cam->yaw -= cam->rot_speed;
}

static void camera_rotate_pitch(struct camera *cam, float seconds, int direction)
{
    if (direction > 0) {
        if (cam->pitch < M_PI / 2 - cam->rot_speed)
            cam->pitch += cam->rot_speed;
    } else {
        if (cam->pitch > -M_PI / 2 + cam->rot_speed)
            cam->pitch -= cam->rot_speed;
    }
}

void camera_rotate_pitch_up(struct camera *cam, float seconds)
{
    camera_rotate_pitch(cam, seconds, (cam->invert_pitch)? 1: -1);
}

void camera_rotate_pitch_down(struct camera *cam, float seconds)
{
    camera_rotate_pitch(cam, seconds, (cam->invert_pitch)? -1: 1);
}

