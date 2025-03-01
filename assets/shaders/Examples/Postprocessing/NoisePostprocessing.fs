#version 330 core

in vec2 TexCoords;

uniform sampler2D sceneTexture;
uniform float intensity;
uniform float et;

out vec4 FragColor;

// Función de ruido aleatorio con variación temporal
float random(vec2 p)
{
    return fract(sin(dot(p + et * 0.1, vec2(12.9898, 78.233))) * 43758.5453);
}

void main()
{
    // Desplazamiento aleatorio de coordenadas con variación en el tiempo de ejecución
    vec2 noiseOffset = vec2(random(TexCoords + et), random(TexCoords - et)) * intensity * 0.01;
    vec2 newCoords = TexCoords + noiseOffset;

    // Obtener color de la textura con coordenadas perturbadas
    vec4 color = texture(sceneTexture, newCoords);

    FragColor = color;
}