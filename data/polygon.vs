#version 450 core

layout (location = 0) in vec2 position;
uniform vec3 color;

out vec3 f_Color;

void main()
{
    gl_Position = vec4(position.x, position.y, 0.0, 1.0);
    f_Color = color;
};
