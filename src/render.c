
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "list.h"
#include "gl_math.h"
#include "map.h"
#include "cube.h"
#include "shaders.h"
#include "camera.h"
#include "render.h"

struct render_state {
    GLFWwindow *window;
    struct camera camera;

    list_head_t element_list;
};

GLFWwindow *window;

static struct render_state render_state = {
    .window = NULL,
    .camera = {
        .pos = { .x = 0, .y = 0, .z = 0 },
        .direction = { .x = 0, .y = 0, .z = 0 },
        .up = { .x = 0, .y = 1, .z = 0 },
        .pitch = 0, .yaw = 0,

        .fov = 60, .aspect = 640.f / 480.f,
        .min_depth = .1f, .max_depth = 100.f,

        .rot_speed = M_PI / 96,
        .cam_speed = .03f,

        .invert_pitch = 1,
        .flat_movement = 1,
    },
    .element_list = LIST_HEAD_INIT(render_state.element_list),
};

/*
static GLFWwindow *window;
static list_head_t element_list = LIST_HEAD_INIT(element_list);
 */

void render_element_add(struct render_element *element)
{
    glGenBuffers(1, &element->buffer_id);
    glBindBuffer(GL_ARRAY_BUFFER, element->buffer_id);

    glGenVertexArrays(1, &element->vertex_arr_id);
    glBindVertexArray(element->vertex_arr_id);
    glEnableVertexAttribArray(element->vertex_arr_id);
    glBindBuffer(GL_ARRAY_BUFFER, element->buffer_id);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    list_add(&render_state.element_list, &element->element_node);
}

static void render_element(struct render_state *state, struct render_element *element)
{
    if (element->update_tries)
        (element->update_tries) (element);

    glUseProgram(element->shader_program);

    glUniformMatrix4fv(element->model_uniform_id, 1, GL_TRUE, &element->model.m[0][0]);

    glUniformMatrix4fv(element->proj_uniform_id, 1, GL_TRUE, &state->camera.proj.m[0][0]);
    glUniformMatrix4fv(element->view_uniform_id, 1, GL_TRUE, &state->camera.view.m[0][0]);

    glBindVertexArray(element->vertex_arr_id);
    glDrawArrays(GL_TRIANGLES, 0, element->cur_tri_count * 3);
}

static void render_elements(struct render_state *state)
{
    struct render_element *element;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    list_foreach_entry(&state->element_list, element, element_node)
        render_element(state, element);
}

void render_main_loop(struct map *map)
{
    struct tri *map_tris = NULL;
    int tri_count = 0;
    GLuint map_buffer_id;
    GLuint map_vertex_arr;
    GLuint map_prog;
    GLuint model_uniform, view_uniform, proj_uniform;
    struct camera camera = {
        .pos = { .x = map->startx + .5f, .y = .5f, .z = map->startz + .5f },
        .direction = { .x = 0, .y = 0, .z = 0 },
        .up = { .x = 0, .y = 1, .z = 0 },
        .pitch = 0, .yaw = 0,

        .fov = 60, .aspect = 640.f / 480.f,
        .min_depth = .1f, .max_depth = 100.f,

        .rot_speed = M_PI / 96,
        .cam_speed = .03f,

        .invert_pitch = 1,
        .flat_movement = 1,
    };
    int frames = 0;
    float prev_time;
    struct mat4 model;

    glGenBuffers(1, &map_buffer_id);
    glBindBuffer(GL_ARRAY_BUFFER, map_buffer_id);

    glGenVertexArrays(1, &map_vertex_arr);
    glBindVertexArray(map_vertex_arr);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, map_buffer_id);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    map_render(map, &map_tris, &tri_count);

    glBufferData(GL_ARRAY_BUFFER, tri_count * sizeof(struct tri), map_tris, GL_STATIC_DRAW);

    printf("tri_count: %d\n", tri_count);

    map_prog = create_shader(&map_vertex_shader, &map_fragment_shader);

    glUseProgram(map_prog);

    view_uniform = glGetUniformLocation(map_prog, "view");
    proj_uniform = glGetUniformLocation(map_prog, "proj");
    model_uniform = glGetUniformLocation(map_prog, "model");

    mat4_make_identity(&model);
    camera_recalc(&camera);

    prev_time = glfwGetTime();

    double old_xpos = 0, old_ypos = 0;
    int cursor_disabled = 0;
    while (!glfwWindowShouldClose(window)) {
        int recalc = 0;
        struct key_callback {
            int key;
            void (*callback) (struct camera *, float seconds);
        } key_handlers[] = {
            { GLFW_KEY_A, camera_move_left },
            { GLFW_KEY_D, camera_move_right },
            { GLFW_KEY_W, camera_move_forward },
            { GLFW_KEY_S, camera_move_backward },
            { GLFW_KEY_DOWN, camera_rotate_pitch_up },
            { GLFW_KEY_UP, camera_rotate_pitch_down },
            { GLFW_KEY_LEFT, camera_rotate_yaw_up },
            { GLFW_KEY_RIGHT, camera_rotate_yaw_down },
            { 0, NULL }
        };
        struct key_callback *handler;
        double xpos, ypos;

        for (handler = key_handlers; handler->callback; handler++) {
            if (glfwGetKey(window, handler->key)) {
                (handler->callback) (&camera, 1.f / 60.f);
                recalc = 1;
            }
        }

        if (glfwGetKey(window, GLFW_KEY_M)) {
            if (!cursor_disabled) {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                cursor_disabled = 1;
            } else {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                cursor_disabled = 0;
            }
        }

        glfwGetCursorPos(window, &xpos, &ypos);

        if (xpos != old_xpos) {
            double diff = xpos - old_xpos;
            if (diff < 0)
                camera_rotate_yaw_up(&camera, diff / 60);
            else
                camera_rotate_yaw_down(&camera, diff / 60);

            recalc = 1;
            old_xpos = xpos;
        }

        if (ypos != old_ypos) {
            double diff = ypos - old_ypos;
            if (diff < 0)
                camera_rotate_pitch_down(&camera, diff / 60);
            else
                camera_rotate_pitch_up(&camera, diff / 60);

            recalc = 1;
            old_ypos = ypos;
        }


        if (recalc)
            camera_recalc(&camera);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(map_prog);

        glUniformMatrix4fv(model_uniform, 1, GL_TRUE, &model.m[0][0]);
        glUniformMatrix4fv(proj_uniform, 1, GL_TRUE, &camera.proj.m[0][0]);
        glUniformMatrix4fv(view_uniform, 1, GL_TRUE, &camera.view.m[0][0]);

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

