
#include <stdio.h>
#include <math.h>

#include "render.h"
#include "map.h"
#include "cone.h"
#include "rotate.h"
#include "shaders.h"
#include "gl_math.h"

/* This define is just used to make the map a little more readable */
#define W 1

static struct map map = {
    .width = 20,
    .height = 20,
    .startx = 1,
    .startz = 1,
    .finishx = 18,
    .finishz = 18,
    .walls = (int[]) {
        W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W,
        W, 0, 0, 0, 0, 0, 0, 0, 0, W, W, W, 0, 0, 0, 0, 0, 0, 0, W,
        W, 0, W, W, W, W, 0, W, 0, W, 0, 0, 0, 0, W, W, W, W, W, W,
        W, 0, 0, 0, W, W, 0, 0, 0, W, 0, 0, W, 0, W, W, 0, 0, 0, W,
        W, W, W, 0, W, 0, W, 0, 0, W, 0, W, W, 0, W, W, 0, W, 0, W,
        W, W, W, 0, W, 0, W, 0, W, W, W, W, W, 0, W, W, 0, W, 0, W,
        W, 0, 0, 0, W, 0, 0, 0, 0, W, W, 0, 0, 0, 0, 0, 0, W, 0, W,
        W, 0, W, W, W, W, 0, W, 0, 0, W, 0, W, 0, W, W, W, W, 0, W,
        W, 0, 0, 0, 0, W, 0, 0, 0, 0, 0, 0, 0, 0, W, W, 0, 0, 0, W,
        W, 0, W, W, 0, 0, 0, W, W, W, W, W, W, W, W, 0, 0, W, W, W,
        W, 0, 0, W, W, W, W, W, W, W, W, W, W, W, W, 0, 0, W, W, W,
        W, W, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, W, 0, W, 0, 0, W,
        W, W, W, 0, W, W, W, 0, W, W, W, W, W, 0, 0, 0, W, W, 0, W,
        W, 0, 0, 0, W, W, 0, 0, 0, 0, 0, 0, W, 0, W, 0, 0, 0, 0, W,
        W, W, W, 0, W, 0, W, 0, 0, W, W, 0, W, 0, W, 0, W, W, 0, W,
        W, W, W, 0, W, 0, W, 0, W, W, W, W, W, 0, W, W, W, W, W, W,
        W, 0, 0, 0, W, 0, 0, W, 0, 0, 0, 0, W, 0, 0, 0, 0, 0, 0, W,
        W, 0, W, W, W, W, 0, 0, 0, W, W, 0, 0, 0, W, W, 0, W, W, W,
        W, 0, 0, 0, W, W, W, W, W, W, W, 0, W, 0, W, W, 0, 0, 0, W,
        W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W,
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

        .fov = 60, .aspect = 640.f / 480.f,
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
    .element_node = LIST_NODE_INIT(map_element.element_node),
    .model = MAT4_IDENTITY(),
};

static struct render_element ceiling_element = {
    .element_node = LIST_NODE_INIT(map_element.element_node),
    .model = MAT4_IDENTITY(),
};

static struct render_element cone_element  = {
    .element_node = LIST_NODE_INIT(map_element.element_node),
    .model = MAT4_IDENTITY(),
    .update_tries = render_element_rotate,
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

static void place_at_end(const struct map *map, struct render_element *element)
{
    mat4_make_translation(&element->model, &(struct vec3) { .x = map->finishx + .5f, .y = .0f, .z = map->finishz + .5f });
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

    map_render(&map, &map_element.cur_buf, &map_element.cur_tri_count);
    setup_map_element(&map_element, &map_vertex_shader, &map_fragment_shader);

    map_render_floor(&map, &floor_element.cur_buf, &floor_element.cur_tri_count);
    setup_map_element(&floor_element, &floor_vertex_shader, &floor_fragment_shader);

    map_render_ceiling(&map, &ceiling_element.cur_buf, &ceiling_element.cur_tri_count);
    setup_map_element(&ceiling_element, &ceiling_vertex_shader, &ceiling_fragment_shader);

    cone_render(&cone_element.cur_buf, &cone_element.cur_tri_count, .1f, .5f, .2f, .8f, 100);
    setup_map_element(&cone_element, &cone_vertex_shader, &cone_fragment_shader);
    place_at_end(&map, &cone_element);

    render_state.camera.pos.x = map.startx + .5f;
    render_state.camera.pos.y = .5f;
    render_state.camera.pos.z = map.startz + .5f;

    render_main_loop(&render_state);

    return renderer_end(&render_state);
}
