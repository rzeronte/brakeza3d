#version 330 core

in vec2 TexCoords;

uniform sampler2D image;
uniform float alpha;

out vec4 color;

void main()
{
    color = vec4(vec3(1), alpha) * texture(image, TexCoords);
}