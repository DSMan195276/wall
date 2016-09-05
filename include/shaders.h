#ifndef INCLUDE_SHADERS_H
#define INCLUDE_SHADERS_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

const char *map_vertex_shader;
const char *map_fragment_shader;

GLuint create_shader(const char **vert_shad, const char **frag_shad);

#endif
