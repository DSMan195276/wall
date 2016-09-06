#version 150

in vec3 vp;

uniform mat4 view;
uniform mat4 proj;
uniform mat4 model;

void main()
{
    gl_Position = proj * view * model * vec4(vp, 1);
}

