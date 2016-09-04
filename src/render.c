
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "gl_math.h"
#include "map.h"
#include "shaders.h"
#include "render.h"

struct render {
    GLFWwindow *window;
};

static GLFWwindow *window;

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

/* tri should contain steps * 2 triangles */
void fill_cone(struct tri *tri, float base_rad, float top_rad, float height, int steps)
{
    int i;
    float dtheta = (M_PI * 2) / steps;
    float theta = 0;

    for (i = 0; i < steps * 2; i += 2) {
        tri[i + 0].p1 = (struct vec3) { .x = cosf(theta) * top_rad,           .y = height / 2, .z = sinf(theta) * top_rad };
        tri[i + 0].p2 = (struct vec3) { .x = cosf(theta + dtheta) * top_rad,  .y = height / 2, .z = sinf(theta + dtheta) * top_rad };
        tri[i + 0].p3 = (struct vec3) { .x = cos(theta) * base_rad,           .y = -height / 2,      .z = sinf(theta) * base_rad };

        tri[i + 1].p1 = (struct vec3) { .x = cosf(theta + dtheta) * top_rad,  .y = height / 2, .z = sinf(theta + dtheta) * top_rad };
        tri[i + 1].p2 = (struct vec3) { .x = cosf(theta) * base_rad,          .y = -height/2,      .z = sinf(theta) * base_rad};
        tri[i + 1].p3 = (struct vec3) { .x = cosf(theta + dtheta) * base_rad, .y = -height/2,      .z = sinf(theta + dtheta) * base_rad };

        theta += dtheta;
    }
}

void render_main_loop(struct map *map)
{
    struct tri *map_tris = NULL;
    int tri_count = 0;
    GLuint map_buffer_id;
    GLuint map_vertex_arr;
    GLuint map_vs, map_fs;
    GLuint map_prog;
    GLuint rot_uniform, scale_uniform, view_uniform, proj_uniform, model_uniform;
    int frames = 0;
    float prev_time;
    struct mat4 view, proj, model;
    int steps = 10;

    glGenBuffers(1, &map_buffer_id);
    glBindBuffer(GL_ARRAY_BUFFER, map_buffer_id);

    glGenVertexArrays(1, &map_vertex_arr);
    glBindVertexArray(map_vertex_arr);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, map_buffer_id);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    map_render(map, &map_tris, &tri_count);
#if 0
    tri_count = steps * 2;
    map_tris = malloc(sizeof(struct tri) * tri_count);
    fill_cone(map_tris, .7f, .3f, .5f, steps);
#endif

    glBufferData(GL_ARRAY_BUFFER, tri_count * sizeof(struct tri), map_tris, GL_STATIC_DRAW);

    printf("tri_count: %d\n", tri_count);

    map_vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(map_vs, 1, &map_vertex_shader, NULL);
    glCompileShader(map_vs);

    map_fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(map_fs, 1, &map_fragment_shader, NULL);
    glCompileShader(map_fs);

    map_prog = glCreateProgram();
    glAttachShader(map_prog, map_vs);
    glAttachShader(map_prog, map_fs);
    glLinkProgram(map_prog);

    glUseProgram(map_prog);

    view_uniform = glGetUniformLocation(map_prog, "view");
    proj_uniform = glGetUniformLocation(map_prog, "proj");
    model_uniform = glGetUniformLocation(map_prog, "model");

    struct vec3 cam_pos = { .x = -1, .y = .5f, .z = -1 };
    struct vec3 cam_direction = { .x = 0, .y = 0, .z = 0 };
    float cam_pitch = 0, cam_yaw = .0f;
    /* struct vec4 cam_direction = { .x = .0f, .y = .0f, .z = .0f }; */
    /* struct vec3 target_pos = { .x = .0f, .y = .0f, .z = .0f }; */
    struct vec3 cam_up = { .x = 0, .y = 1, .z = 0 };
    float rot_speed = M_PI / 64.f;
    float cam_speed = .03f;

    prev_time = glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
#if 0
        if (glfwGetKey(window, GLFW_KEY_W)) {
            cam_pos = (struct vec3) { .x = cam_pos.x + target_pos.x * .01f, .y = cam_pos.y + target_pos.y * .01f, .z = cam_pos.z + target_pos.z * .01f };
        }

        if (glfwGetKey(window, GLFW_KEY_S))
            cam_pos = (struct vec3) { .x = cam_pos.x - target_pos.x * .01f, .y = cam_pos.y - target_pos.y * .01f, .z = cam_pos.z - target_pos.z * .01f };

        if (glfwGetKey(window, GLFW_KEY_W)) {
            struct vec3 tmp;
            vec3_cross(&tmp, &target_pos, &cam_up);
            vec3_normalize(&tmp);
            cam_pos = (struct vec3) { .x = cam_pos.x + tmp.x * .01f, .y = cam_pos.y + tmp.y * .01f, .z = cam_pos.z + tmp.z * .01f };
        }

        if (glfwGetKey(window, GLFW_KEY_S)) {
            struct vec3 tmp;
            vec3_cross(&tmp, &target_pos, &cam_up);
            vec3_normalize(&tmp);
            cam_pos = (struct vec3) { .x = cam_pos.x - tmp.x * .01f, .y = cam_pos.y - tmp.y * .01f, .z = cam_pos.z - tmp.z * .01f };
        }
#endif
        if (glfwGetKey(window, GLFW_KEY_A)) {
            struct vec3 tmp, move, res;
            vec3_cross(&tmp, &cam_direction, &cam_up);
            vec3_normalize(&tmp);

            vec3_mul_scaler(&move, &tmp, -cam_speed);
            vec3_add(&res, &cam_pos, &move);
            memcpy(&cam_pos, &res, sizeof(cam_pos));
            //cam_pos.z += .01;
        }

        if (glfwGetKey(window, GLFW_KEY_D)) {
            struct vec3 tmp, move, res;
            vec3_cross(&tmp, &cam_direction, &cam_up);
            vec3_normalize(&tmp);

            vec3_mul_scaler(&move, &tmp, cam_speed);
            vec3_add(&res, &cam_pos, &move);
            memcpy(&cam_pos, &res, sizeof(cam_pos));
            //cam_pos.z -= .01;
        }

        if (glfwGetKey(window, GLFW_KEY_W)) {
            struct vec3 move, res;
            vec3_mul_scaler(&move, &cam_direction, cam_speed);
            vec3_add(&res, &cam_pos, &move);
            memcpy(&cam_pos, &res, sizeof(cam_pos));
            //cam_pos.x -= .01;
        }

        if (glfwGetKey(window, GLFW_KEY_S)) {
            struct vec3 move, res;
            vec3_mul_scaler(&move, &cam_direction, -cam_speed);
            vec3_add(&res, &cam_pos, &move);
            memcpy(&cam_pos, &res, sizeof(cam_pos));
            //cam_pos.x += .01;
        }

        if (glfwGetKey(window, GLFW_KEY_DOWN)) {
            if (cam_pitch < M_PI / 2 - rot_speed)
                cam_pitch += rot_speed;
        }

        if (glfwGetKey(window, GLFW_KEY_UP)) {
            if (cam_pitch > -M_PI / 2 + rot_speed)
                cam_pitch -= rot_speed;
        }

        if (glfwGetKey(window, GLFW_KEY_LEFT)) {
            cam_yaw += rot_speed;
        }

        if (glfwGetKey(window, GLFW_KEY_RIGHT)) {
            cam_yaw -= rot_speed;
        }

#if 0
        if (glfwGetKey(window, GLFW_KEY_UP)) {
            struct mat4 rot;
            struct vec4 vec;
            mat4_make_rotation(&rot, M_PI / 64, ROT_Y);
            mat4_mult_vec4(&vec, &rot, &(struct vec4) { .x = target_pos.x, .y = target_pos.y, .z = target_pos.z, .w = 1 });
            target_pos = (struct vec3) { .x = vec.x, .y = vec.y, .z = vec.z };
            //cam_pos.y += .01;
            //target_pos.y += .01;
        }
            //xtheta += (M_PI / 64);

        if (glfwGetKey(window, GLFW_KEY_DOWN)) {
            struct mat4 rot;
            struct vec4 vec;
            mat4_make_rotation(&rot, -M_PI / 64, ROT_Y);
            mat4_mult_vec4(&vec, &rot, &(struct vec4) { .x = target_pos.x, .y = target_pos.y, .z = target_pos.z, .w = 1 });
            target_pos = (struct vec3) { .x = vec.x, .y = vec.y, .z = vec.z };
            //cam_pos.y -= .01;
            //target_pos.y -= .01;
        }
            //xtheta -= (M_PI / 64);

        if (glfwGetKey(window, GLFW_KEY_LEFT))
            //ytheta -= (M_PI / 64);

        if (glfwGetKey(window, GLFW_KEY_RIGHT))
            //ytheta += (M_PI / 64);
#endif

        /*
        //mat4_make_translation(&tmp1, &cam_pos);
        mat4_make_rotation(&tmp2, xtheta, ROT_X);

        //mat4_mult(&tmp3, &tmp1, &tmp2);

        mat4_make_rotation(&tmp1, ytheta, ROT_Y);

        mat4_mult(&rot, &tmp1, &tmp2);

        struct vec4 nt;

        mat4_mult_vec4(&nt, &rot, &(struct vec4) { .x = target_pos.x, .y = target_pos.y, .z = target_pos.z, .w = 0 }); */

        //mat4_make_scale(&scale, &(struct vec3) { .x = fscale, .y = fscale, .z = fscale });
        //mat4_make_rotation(&scale, M_PI / 4, ROT_X);
        //mat4_make_identity(&proj);
        //mat4_make_rotation(&model, M_PI / 4, ROT_X);
        mat4_make_identity(&model);

        //cam_pos = (struct vec3) { .x = sinf(glfwGetTime()) * .5f, .z = cosf(glfwGetTime()) * .5f, .y = .0f };

        struct vec3 target_pos;
        cam_direction = (struct vec3) { .x = (cosf(cam_pitch) * sinf(cam_yaw)),
                                        .y = sinf(cam_pitch),
                                        .z = (cosf(cam_pitch) * cosf(cam_yaw)) };
        /*
        cam_direction = (struct vec3) { .x = cosf(cam_pitch),
                                        .y = sinf(cam_pitch),
                                        .z = cosf(cam_pitch)};
        cam_direction = (struct vec3) { .x = cosf(cam_yaw),
                                        .y = 0,
                                        .z = sinf(cam_yaw) };
         */
        vec3_normalize(&cam_direction);

        printf("Pitch: %f, Yaw: %f\n", cam_pitch, cam_yaw);
        printf("cam_direction: ");
        print_vec3(&cam_direction);

        vec3_add(&target_pos, &cam_pos, &cam_direction);

        printf("Target_pos: ");
        print_vec3(&target_pos);

        mat4_look_at(&view, &cam_pos, &target_pos, &cam_up);

        mat4_make_perspective(&proj, 60, 640.f / 480.f, .1f, 100.f);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(map_prog);

        glUniformMatrix4fv(model_uniform, 1, GL_TRUE, &model.m[0][0]);
        glUniformMatrix4fv(proj_uniform, 1, GL_TRUE, &proj.m[0][0]);
        glUniformMatrix4fv(view_uniform, 1, GL_TRUE, &view.m[0][0]);

        glBindVertexArray(map_vertex_arr);
        glDrawArrays(GL_TRIANGLES, 0, tri_count * 3);

        glfwPollEvents();

        glfwSwapBuffers(window);

        frames++;
        if (frames == 60) {
            float f = glfwGetTime();
            printf("FPS: %f\n", 60 / (f - prev_time));
            prev_time = f;
            frames = 0;
        }
    }
}

int renderer_start(void)
{
    if (!glfwInit()) {
        fprintf(stderr, "Error: GLFW3 would not start\n");
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(640, 480, "Wall!", NULL, NULL);
    if (!window) {
        fprintf(stderr, "ERROR: Could not create window\n");
        return 1;
    }

    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    glewInit();

    const unsigned char *renderer = glGetString(GL_RENDERER);
    const unsigned char *version = glGetString(GL_VERSION);
    int attribute_count;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &attribute_count);

    printf("Renderer: %s\n", renderer);
    printf("OpenGL version supported: %s\n", version);
    printf("Max vertex attributes: %d\n", attribute_count);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
#if 0
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
#endif

    return 0;
}

int renderer_end(void)
{
    glfwTerminate();

    return 0;
}

