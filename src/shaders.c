
#include <GL/glew.h>
#include <GLFW/glfw3.h>

GLuint create_shader(const char **vert_shad, const char **frag_shad)
{
    GLuint vs, fs, prog;

    vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, vert_shad, NULL);
    glCompileShader(vs);

    fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, frag_shad, NULL);
    glCompileShader(fs);

    prog = glCreateProgram();
    glAttachShader(prog, vs);
    glAttachShader(prog, fs);
    glLinkProgram(prog);

    return prog;
}

#define SHADER_DEFINE(shader) \
    const char *shader = (char[])

SHADER_DEFINE(map_vertex_shader) {
# include "shaders/map_shader.vx"
};

SHADER_DEFINE(map_fragment_shader) {
# include "shaders/map_shader.fx"
};

SHADER_DEFINE(floor_vertex_shader) {
# include "shaders/floor_shader.vx"
};

SHADER_DEFINE(floor_fragment_shader) {
# include "shaders/floor_shader.fx"
};

SHADER_DEFINE(ceiling_vertex_shader) {
# include "shaders/ceiling_shader.vx"
};

SHADER_DEFINE(ceiling_fragment_shader) {
# include "shaders/ceiling_shader.fx"
};

SHADER_DEFINE(cone_vertex_shader) {
# include "shaders/cone_shader.vx"
};

SHADER_DEFINE(cone_fragment_shader) {
# include "shaders/cone_shader.fx"
};

