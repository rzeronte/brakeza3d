#version 330 core

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D sceneTexture;
uniform sampler2D depthTexture;

uniform float farPlane;
uniform float intensity;

uniform float fogMaxDist;
uniform float fogMinDist;
uniform vec3 fogColor;

float nearPlane = 0.1f;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0;
    return (2.0 * nearPlane * farPlane) / (farPlane + nearPlane - z * (farPlane - nearPlane));
}

void main()
{
    float depthValue = texture(depthTexture, TexCoords).r * intensity;
    float linearDepth = LinearizeDepth(depthValue) / farPlane;

    float fogFactor = smoothstep(fogMinDist / farPlane, fogMaxDist / farPlane, linearDepth);

    // Mezcla el color original con el color de la niebla
    vec3 finalColor = mix(texture(sceneTexture, TexCoords).rgb, fogColor, fogFactor);

    FragColor = vec4(finalColor, 1.0);
}