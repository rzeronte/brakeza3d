#version 330 core

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D depthTexture;

void main()
{
    float depth = texture(depthTexture, TexCoords).r;  // ← Solo vec2, sin la coordenada z
    FragColor = vec4(vec3(depth), 1.0);
}