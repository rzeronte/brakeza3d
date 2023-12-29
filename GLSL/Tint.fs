#version 330 core

in vec2 TexCoords;

out vec4 FragColor;

uniform vec3 color;
uniform float alpha;

void main()
{
    FragColor = vec4(color, alpha);
}