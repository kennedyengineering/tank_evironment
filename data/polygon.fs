#version 450 core

in vec3 f_Color;

out vec4 frag_Color;

void main()
{
    frag_Color = vec4(f_Color.rgb, 1.0f);
};
