#version 330 core

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D depthTexture;

float nearPlane = 0.0;
float farPlane = 20.0;
float intensity = 1.0;

void main()
{
    float depth = texture(depthTexture, TexCoords).r;  // ← Solo vec2, sin la coordenada z
    FragColor = vec4(vec3(depth), 1.0);
}