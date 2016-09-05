#ifndef INCLUDE_RENDER_H
#define INCLUDE_RENDER_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "list.h"
#include "gl_math.h"
#include "map.h"

struct render_element {
    list_node_t element_node; /* List of elements */

    /* 
     * A malloc'd buffer of 'struct tri' objects.
     *
     * update_tries edits and resizes this buffer.
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

void render_cube(struct tri *tri, struct cube *cube);
void render_main_loop(struct map *map);

void render_element_add(struct render_element *);

int renderer_start(void);
int renderer_end(void);

#endif
