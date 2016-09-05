#ifndef INCLUDE_CAMERA_H
#define INCLUDE_CAMERA_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "gl_math.h"

struct camera {
    struct vec3 pos, direction, up;
    float pitch, yaw;
    float fov, aspect;
    float min_depth, max_depth;

    float rot_speed, cam_speed;

    int invert_pitch :1;
    int flat_movement :1; /* 'move' functions will not be affected by 'y' direction */

    struct mat4 view, proj;
};

void camera_init(struct camera *);
void camera_recalc(struct camera *);

void camera_move_forward(struct camera *, float seconds);
void camera_move_backward(struct camera *, float seconds);
void camera_move_left(struct camera *, float seconds);
void camera_move_right(struct camera *, float seconds);

void camera_rotate_yaw_up(struct camera *, float seconds);
void camera_rotate_yaw_down(struct camera *, float seconds);

void camera_rotate_pitch_up(struct camera *, float seconds);
void camera_rotate_pitch_down(struct camera *, float seconds);

#endif
