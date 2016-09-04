#version 150

#define PI 3.1415927653

in vec4 color;

out vec4 frag_color;

void main()
{
    frag_color = vec4(color.r, color.g, color.b, 1);
}

