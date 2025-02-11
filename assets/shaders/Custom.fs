#version 330 core

in vec2 TexCoords;

uniform sampler2D sceneTexture;

uniform float r_factor;
uniform float g_factor;
uniform float b_factor;
uniform float a_factor;

out vec4 FragColor;

void main()
{
    // FragColor = texture(sceneTexture, TexCoords);
    FragColor = vec4(r_factor, g_factor, b_factor, a_factor);
}