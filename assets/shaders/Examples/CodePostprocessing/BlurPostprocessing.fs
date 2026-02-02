#version 330 core

in vec2 TexCoords;

uniform sampler2D sceneTexture;
uniform float intensity; // Controla la cantidad de desenfoque

out vec4 FragColor;

void main()
{
    float offset = intensity * 0.02; // Escala reducida para evitar muestreo fuera de la imagen

    // Kernel de desenfoque 3x3
    vec4 sum = vec4(0.0);
    sum += texture(sceneTexture, TexCoords + vec2(-offset, -offset)) * 0.111;
    sum += texture(sceneTexture, TexCoords + vec2( 0.0, -offset)) * 0.111;
    sum += texture(sceneTexture, TexCoords + vec2( offset, -offset)) * 0.111;
    sum += texture(sceneTexture, TexCoords + vec2(-offset,  0.0)) * 0.111;
    sum += texture(sceneTexture, TexCoords) * 0.111;
    sum += texture(sceneTexture, TexCoords + vec2( offset,  0.0)) * 0.111;
    sum += texture(sceneTexture, TexCoords + vec2(-offset,  offset)) * 0.111;
    sum += texture(sceneTexture, TexCoords + vec2( 0.0,  offset)) * 0.111;
    sum += texture(sceneTexture, TexCoords + vec2( offset,  offset)) * 0.111;

    // Mantener el alpha original
    FragColor = vec4(sum.rgb, texture(sceneTexture, TexCoords).a);
}
