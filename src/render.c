
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

#define USE_MOUSE 1

void render_element_add(struct render_state *state, struct render_element *element)
{
    glGenBuffers(1, &element->buffer_id);
    glBindBuffer(GL_ARRAY_BUFFER, element->buffer_id);

    glGenVertexArrays(1, &element->vertex_arr_id);
    glBindVertexArray(element->vertex_arr_id);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, element->buffer_id);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    list_add(&state->element_list, &element->element_node);
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

    list_foreach_entry(&state->element_list, element, element_node)
        render_element(state, element);
}

static void render_frame(struct render_state *state)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    render_elements(state);

    glfwSwapBuffers(state->window);

    glfwPollEvents();
}

void render_mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    struct render_state *state = glfwGetWindowUserPointer(window);
    double diffx = xpos - state->m_xpos;
    double diffy = ypos - state->m_ypos;

    state->m_xpos = xpos;
    state->m_ypos = ypos;

    printf("xpos: %f, ypos: %f\n", xpos, ypos);

    if (diffx)
        state->camera.yaw = -xpos / 1000;

    if (diffy)
        state->camera.pitch = ypos / 1000;

    camera_recalc(&state->camera);
}

void render_main_loop(struct render_state *state)
{
    int frames = 0;
    float prev_time;

    camera_recalc(&state->camera);

    prev_time = glfwGetTime();

    glfwSetWindowUserPointer(state->window, state);
    glfwSetCursorPosCallback(state->window, render_mouse_callback);

#if USE_MOUSE
    int cursor_disabled = 0, m_pressed = 0;
#endif
    while (!glfwWindowShouldClose(state->window)) {
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

        for (handler = key_handlers; handler->callback; handler++) {
            if (glfwGetKey(state->window, handler->key)) {
                (handler->callback) (&state->camera, 1.f / 60.f);
                recalc = 1;
            }
        }

#if USE_MOUSE
        if (glfwGetKey(state->window, GLFW_KEY_M) && !m_pressed) {
            if (!cursor_disabled) {
                glfwSetInputMode(state->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                cursor_disabled = 1;
            } else {
                glfwSetInputMode(state->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                cursor_disabled = 0;
            }
            m_pressed = 1;
        } else if (!glfwGetKey(state->window, GLFW_KEY_M)) {
            m_pressed = 0;
        }
#endif

        if (recalc)
            camera_recalc(&state->camera);

        render_frame(state);

        frames++;
        if (frames == 60) {
            float f = glfwGetTime();
            printf("FPS: %f\n", 60 / (f - prev_time));
            printf("Position: "PRvec3"\n", Pvec3(&state->camera.pos));
            prev_time = f;
            frames = 0;
        }
    }
}

int renderer_start(struct render_state *state)
{
    if (!glfwInit()) {
        fprintf(stderr, "Error: GLFW3 would not start\n");
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    state->window = glfwCreateWindow(1024, 768, "Wall!", NULL, NULL);
    if (!state->window) {
        fprintf(stderr, "ERROR: Could not create window\n");
        return 1;
    }

    glfwMakeContextCurrent(state->window);

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

int renderer_end(struct render_state *state)
{
    glfwTerminate();

    return 0;
}

