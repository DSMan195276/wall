
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

const char *map_vertex_shader = (char[]) {
#include "shaders/map_shader.vx"
, '\0'
};

const char *map_fragment_shader = (char[]) {
#include "shaders/map_shader.fx"
, '\0'
};

