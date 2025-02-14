#version 330 core

in vec2 TexCoords;  // Coordenadas de la textura normalizadas (0,1)

uniform sampler2D sceneTexture;
uniform sampler2D noise;

uniform vec2 sceneTextureSize;  // Tamaño de la textura principal
uniform vec2 noiseTextureSize;    // Tamaño de la textura "esa"

uniform float r_factor;
uniform float g_factor;
uniform float b_factor;
uniform float a_factor;

out vec4 FragColor;

void main()
{
    // Color de la textura principal (sceneTexture)
    vec4 color1 = texture(sceneTexture, TexCoords);

    // Ajustar coordenadas de textura para 'esa'
    vec2 noiseCoords = TexCoords * (sceneTextureSize / noiseTextureSize);
    vec4 color2 = texture(noise, noiseCoords);

    // Fusionar las dos texturas (por ejemplo, mezcla al 50%)
    vec4 blendedColor = mix(color1, color2, 0.9);

    // Aplicar factores de color
    FragColor = blendedColor * vec4(r_factor, g_factor, b_factor, a_factor);
}
