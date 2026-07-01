#version 330 core

in vec2 TexCoords;

uniform sampler2D image;
uniform float alpha;
uniform vec4 tintColor;

out vec4 color;

void main()
{
    color = vec4(tintColor.rgb, alpha) * texture(image, TexCoords);
}