#version 330 core

layout (location = 0) in vec4 aPosition;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 color;

out vec4 fragColor;

void main()
{
    gl_Position = projection * view * model * aPosition;

    gl_PointSize = 2.0f;
    fragColor = vec4(color, 1);
}
