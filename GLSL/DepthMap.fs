#version 330 core

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D depthTexture;
uniform float intensity;
uniform float farPlane;
uniform float nearPlane;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // Convertir de [0,1] a [-1,1] (NDC)
    return (2.0 * nearPlane * farPlane) / (farPlane + nearPlane - z * (farPlane - nearPlane));
}

void main()
{
    float depth = texture(depthTexture, TexCoords).r;  // ← Solo vec2, sin la coordenada z
    FragColor = vec4(vec3(depth), 1.0);
}
