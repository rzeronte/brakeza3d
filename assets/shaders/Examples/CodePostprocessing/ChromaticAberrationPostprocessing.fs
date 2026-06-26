#version 330 core

in vec2 TexCoords;

uniform sampler2D sceneTexture;
uniform float intensity; // Controla la cantidad de aberración

out vec4 FragColor;

void main()
{
    // Desplazamientos de los canales R, G y B
    float offset = intensity * 0.1; // Ajustable para cambiar la intensidad de la aberración

    vec2 texOffset = vec2(offset, 0.0); // Desplaza horizontalmente
    vec2 texOffsetY = vec2(0.0, offset); // Si quieres experimentar con el eje Y también

    // Muestreo de los colores con pequeños desplazamientos
    vec4 center = texture(sceneTexture, TexCoords);
    float r = texture(sceneTexture, TexCoords + texOffset).r;
    float g = center.g;
    float b = texture(sceneTexture, TexCoords - texOffset).b;

    FragColor = vec4(r, g, b, center.a);
}
