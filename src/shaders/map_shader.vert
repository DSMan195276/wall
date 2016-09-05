#version 150

#define PI 3.1415927653

in vec3 vp;

out vec4 color;

uniform mat4 view;
uniform mat4 proj;
uniform mat4 model;

void main()
{
    color = vec4(.5 * abs(cos(vp.x * 2 / PI)) + .2,
                 .4 * abs(cos(vp.y + .5)) + .3,
                 .4 * abs(cos(vp.z * 4 / PI)) + .1,
                 1.0);

    gl_Position = proj * view * model * vec4(vp, 1);
}

