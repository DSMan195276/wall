#version 150

#define PI 3.1415927653

in vec3 vp;

out vec4 color;

uniform mat4 view;
uniform mat4 proj;
uniform mat4 model;

void main()
{
    color = vec4(.7 * cos(vp.x * 2 / PI) + .2, 0.7 * abs(cos(vp.y * 4)) + .3, .7 * sin(vp.z * 2 / PI) + .1, 1.0);

    gl_Position = proj * view * model * vec4(vp, 1);
}

