#ifndef INCLUDE_RENDER_H
#define INCLUDE_RENDER_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "list.h"
#include "gl_math.h"
#include "camera.h"

struct render_state {
    GLFWwindow *window;
    struct camera camera;

    double m_xpos, m_ypos;

    list_head_t element_list;
};

struct render_element {
    list_node_t element_node; /* List of elements */

    /* 
     * A buffer of 'struct tri' objects.
     *
     * update_tries can edit and resizes this buffer.
     *
     * update_tries also calls glBufferData to apply the data to the buffer
     * 'buffer_id'
     */
    struct tri *cur_buf;
    int cur_tri_count;

    void (*update_tries) (struct render_element *);

    GLuint buffer_id;
    GLuint vertex_arr_id;

    /* Information for running the shader for this element */
    GLuint shader_program;
    GLuint model_uniform_id, view_uniform_id, proj_uniform_id;

    /* Matrix to apply to the completed model - passed to the shader */
    struct mat4 model;
};

#define RENDER_ELEMENT_INIT(element) \
    { \
        .element_node = LIST_NODE_INIT((element).element_node), \
        .model = MAT4_IDENTITY(), \
    }

static inline void rener_element_init(struct render_element *element)
{
    *element = (struct render_element)RENDER_ELEMENT_INIT(*element);
}

void render_main_loop(struct render_state *);

void render_element_add(struct render_state *, struct render_element *);

int renderer_start(struct render_state *);
int renderer_end(struct render_state *);

#endif
