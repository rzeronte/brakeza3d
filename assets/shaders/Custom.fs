#version 330 core

in vec2 TexCoords;

uniform sampler2D sceneTexture;

uniform float r_factor;
uniform float g_factor;
uniform float b_factor;

out vec4 FragColor;

void main()
{
    FragColor = vec4(r_factor, g_factor, b_factor, 1); //texture(sceneTexture, TexCoords);
}