#version 330 core
layout (location = 0) in vec4 aPos;

out vec3 FragPos;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * aPos;
}