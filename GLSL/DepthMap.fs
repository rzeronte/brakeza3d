#version 330 core

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D depthTexture;
uniform float near;
uniform float far;
uniform float intensity;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // Convertir de [0,1] a [-1,1]
    return (2.0 * near * far) / (far + near - z * (far - near));
}

void main()
{
    float depth = texture(depthTexture, TexCoords).r;

    float linearDepth = LinearizeDepth(depth);
    float normalizedDepth = linearDepth / far; // Normalizar a [0,1]

    normalizedDepth = normalizedDepth * intensity;
    normalizedDepth = clamp(normalizedDepth, 0.0, 1.0);

    FragColor = vec4(vec3(normalizedDepth), 1.0);
}