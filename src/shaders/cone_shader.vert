#version 150

#define PI 3.1415927653

in vec3 vp;

out vec4 color;

uniform mat4 view;
uniform mat4 proj;
uniform mat4 model;

void main()
{
    color = vec4(.2 * abs(sin(vp.x / .5)) + .1,
                 .4 * abs(sin((vp.y - .2) / .6 * 2)) + .1,
                 .2 * abs(sin(vp.z / .1)) + .1,
                 1.0);

    gl_Position = proj * view * model * vec4(vp, 1);
}

