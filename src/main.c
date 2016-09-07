
#include <stdio.h>
#include <math.h>

#include "render.h"
#include "map.h"
#include "cone.h"
#include "cat.h"
#include "rotate.h"
#include "shaders.h"
#include "gl_math.h"

/* This define is just used to make the map a little more readable */
#define W 1

static struct map map = {
    .width = 20,
    .height = 30,
    .startx = 1,
    .startz = 1,
    .finishx = 1,
    .finishz = 2,
    .walls = (int[]) {
        W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W,
        W, 0, 0, 0, 0, 0, 0, 0, 0, W, W, W, 0, 0, 0, 0, 0, 0, 0, W, W, W, 0, 0, 0, 0, 0, 0, 0, W,
        W, 0, W, W, W, W, 0, W, 0, W, 0, 0, 0, 0, W, W, W, W, W, W, 0, 0, 0, 0, W, W, W, W, W, W,
        W, 0, 0, 0, W, W, 0, 0, 0, W, 0, 0, W, 0, W, W, 0, 0, 0, 0, 0, 0, W, 0, W, W, 0, 0, 0, W,
        W, W, W, 0, W, 0, W, 0, 0, W, 0, W, W, 0, W, W, 0, W, 0, W, 0, W, W, 0, W, W, 0, W, 0, W,
        W, W, W, 0, W, 0, W, 0, W, W, W, W, W, 0, W, W, 0, W, 0, W, W, W, W, 0, W, W, 0, W, 0, W,
        W, 0, 0, 0, W, 0, 0, 0, 0, W, W, 0, 0, 0, 0, 0, 0, W, 0, W, W, 0, 0, 0, 0, 0, 0, W, 0, W,
        W, 0, W, W, W, W, 0, W, 0, 0, W, 0, W, 0, W, W, W, W, 0, W, W, 0, W, 0, W, W, W, W, 0, W,
        W, 0, 0, 0, 0, W, 0, 0, 0, 0, 0, 0, 0, 0, W, W, 0, 0, 0, 0, 0, 0, 0, 0, W, W, 0, 0, 0, W,
        W, 0, W, W, 0, 0, 0, W, W, W, W, W, W, W, W, 0, 0, W, W, W, W, W, W, W, W, 0, 0, W, W, W,
        W, 0, 0, W, W, W, W, W, W, W, W, W, W, W, W, 0, 0, W, W, W, W, W, W, W, W, 0, 0, W, W, W,
        W, W, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, W, 0, W, 0, 0, 0, 0, 0, 0, 0, W, 0, W, 0, 0, W,
        W, W, W, 0, W, W, W, 0, W, W, W, W, W, 0, 0, 0, W, W, 0, W, W, W, W, 0, 0, 0, W, W, 0, W,
        W, 0, 0, 0, W, W, 0, 0, 0, 0, 0, 0, W, 0, W, 0, 0, 0, 0, 0, 0, 0, W, 0, W, 0, 0, 0, 0, W,
        W, W, W, 0, W, 0, W, 0, 0, W, W, 0, W, 0, W, 0, W, W, 0, W, W, 0, W, 0, W, 0, W, W, 0, W,
        W, W, W, 0, W, 0, W, 0, W, W, W, W, W, 0, W, W, W, W, W, W, W, W, W, 0, W, W, W, W, W, W,
        W, 0, 0, 0, W, 0, 0, W, 0, 0, 0, 0, W, 0, 0, 0, 0, 0, 0, 0, 0, 0, W, 0, 0, 0, 0, 0, 0, W,
        W, 0, W, W, W, W, 0, 0, 0, W, W, 0, 0, 0, W, W, 0, W, W, W, W, 0, 0, 0, W, W, 0, W, W, W,
        W, 0, 0, 0, W, W, W, W, W, W, W, 0, W, 0, W, W, 0, 0, 0, W, W, 0, W, 0, W, W, 0, 0, 0, W,
        W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W,
    },
};

/* ADD CONE AT END OF MAZE */

static struct render_state render_state = {
    .window = NULL,
    .camera = {
        .pos = { .x = 0, .y = 0, .z = 0 },
        .direction = { .x = 0, .y = 0, .z = 0 },
        .up = { .x = 0, .y = 1, .z = 0 },
        .pitch = 0, .yaw = 0,

        .fov = 60, .aspect = 1024.f / 768.f,
        .min_depth = .1f, .max_depth = 100.f,

        .rot_speed = M_PI / 96,
        .cam_speed = .03f,

        .invert_pitch = 1,
        .flat_movement = 1,
    },
    .element_list = LIST_HEAD_INIT(render_state.element_list),
};

static struct render_element map_element = {
    .element_node = LIST_NODE_INIT(map_element.element_node),
    .model = MAT4_IDENTITY(),
};

static struct render_element floor_element = {
    .element_node = LIST_NODE_INIT(floor_element.element_node),
    .model = MAT4_IDENTITY(),
};

static struct render_element ceiling_element = {
    .element_node = LIST_NODE_INIT(ceiling_element.element_node),
    .model = MAT4_IDENTITY(),
};

static struct render_element cone_element  = {
    .element_node = LIST_NODE_INIT(cone_element.element_node),
    .model = MAT4_IDENTITY(),
    .update_tries = render_element_rotate,
};

static struct render_element cat_element = {
    .element_node = LIST_NODE_INIT(cat_element.element_node),
    .model = MAT4_IDENTITY(),
};

static void setup_map_element(struct render_element *element, const char **vertex_shader, const char **fragment_shader)
{
    glBindBuffer(GL_ARRAY_BUFFER, element->buffer_id);
    glBufferData(GL_ARRAY_BUFFER, element->cur_tri_count * sizeof(struct tri), element->cur_buf, GL_STATIC_DRAW);

    element->shader_program = create_shader(vertex_shader, fragment_shader);

    element->view_uniform_id = glGetUniformLocation(element->shader_program, "view");
    element->proj_uniform_id = glGetUniformLocation(element->shader_program, "proj");
    element->model_uniform_id = glGetUniformLocation(element->shader_program, "model");
}

static void place_at_location(float x, float z, struct render_element *element)
{
    mat4_make_translation(&element->model, &(struct vec3) { .x = x + .5f, .y = .0f, .z = z + .5f });
}

static void scale_element(struct render_element *element, float scale)
{
    struct mat4 mat;
    mat4_make_scale(&mat, &(struct vec3) { .x = scale, .y = scale, .z = scale });
    mat4_mult(&element->model, &element->model, &mat);
}

static void rotate_element(struct render_element *element, float rotation, int rot_type)
{
    struct mat4 rot;
    mat4_make_rotation(&rot, rotation, rot_type);
    mat4_mult(&element->model, &element->model, &rot);
}

int main(int argc, char **argv)
{
    int ret;

    ret = renderer_start(&render_state);
    if (ret)
        return ret;

    render_element_add(&render_state, &map_element);
    render_element_add(&render_state, &floor_element);
    render_element_add(&render_state, &ceiling_element);
    render_element_add(&render_state, &cone_element);
    render_element_add(&render_state, &cat_element);

    map_render(&map, &map_element.cur_buf, &map_element.cur_tri_count);
    setup_map_element(&map_element, &map_vertex_shader, &map_fragment_shader);

    map_render_floor(&map, &floor_element.cur_buf, &floor_element.cur_tri_count);
    setup_map_element(&floor_element, &floor_vertex_shader, &floor_fragment_shader);

    map_render_ceiling(&map, &ceiling_element.cur_buf, &ceiling_element.cur_tri_count);
    setup_map_element(&ceiling_element, &ceiling_vertex_shader, &ceiling_fragment_shader);

    cone_render(&cone_element.cur_buf, &cone_element.cur_tri_count, .1f, .5f, .2f, .8f, 100);
    setup_map_element(&cone_element, &cone_vertex_shader, &cone_fragment_shader);
    place_at_location(map.finishx, map.finishz, &cone_element);

    cat_render(&cat_element.cur_buf, &cat_element.cur_tri_count);
    setup_map_element(&cat_element, &cat_vertex_shader, &cat_fragment_shader);
    place_at_location(map.finishz, map.finishx, &cat_element);
    scale_element(&cat_element, .5);
    rotate_element(&cat_element, M_PI * 2 / 3, ROT_Y);

    render_state.camera.pos.x = map.startx + .5f;
    render_state.camera.pos.y = .5f;
    render_state.camera.pos.z = map.startz + .5f;

    render_main_loop(&render_state);

    return renderer_end(&render_state);
}
