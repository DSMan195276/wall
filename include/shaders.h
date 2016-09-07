#ifndef INCLUDE_SHADERS_H
#define INCLUDE_SHADERS_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

const char *map_vertex_shader;
const char *map_fragment_shader;

const char *floor_vertex_shader;
const char *floor_fragment_shader;

const char *ceiling_vertex_shader;
const char *ceiling_fragment_shader;

const char *cone_vertex_shader;
const char *cone_fragment_shader;

const char *cat_vertex_shader;
const char *cat_fragment_shader;

GLuint create_shader(const char **vert_shad, const char **frag_shad);

#endif
