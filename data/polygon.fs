#version 450 core

uniform vec3 color;

out vec4 frag_Color;

void main()
{
    frag_Color = vec4(color.rgb, 1.0f);
};
