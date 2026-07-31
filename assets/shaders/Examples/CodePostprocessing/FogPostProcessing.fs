#version 330 core

in  vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D sceneTexture;
uniform sampler2D depthTexture;

uniform float nearPlane;
uniform float farPlane;
uniform float fogMinDist;
uniform float fogMaxDist;
uniform float intensity;
uniform vec3  fogColor;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0;
    return (2.0 * nearPlane * farPlane) / (farPlane + nearPlane - z * (farPlane - nearPlane));
}

void main()
{
    vec2 tc = vec2(TexCoords.x, 1.0 - TexCoords.y);

    vec3 sceneColor = texture(sceneTexture, tc).rgb;

    float depthValue = texture(depthTexture, tc).r;

    // Pixeles de fondo (cielo / fuera de geometria): depth en el far plane -> pasar tal cual
    if (depthValue >= 0.9999) {
        FragColor = vec4(sceneColor, 1.0);
        return;
    }

    float linearDepth = LinearizeDepth(depthValue) / farPlane;

    float fogFactor = clamp(
        smoothstep(fogMinDist / farPlane, fogMaxDist / farPlane, linearDepth) * intensity,
        0.0, 1.0
    );

    // Si el pixel ya es negro (shroud del FOW), no aplicar niebla atmosférica
    float lum = dot(sceneColor, vec3(0.299, 0.587, 0.114));
    if (lum < 0.02) {
        FragColor = vec4(sceneColor, 1.0);
        return;
    }

    FragColor = vec4(mix(sceneColor, fogColor, fogFactor), 1.0);
}
