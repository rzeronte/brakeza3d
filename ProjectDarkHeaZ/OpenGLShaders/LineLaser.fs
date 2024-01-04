#version 330 core

in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D mask;

uniform float maskSpeed;
uniform float maskIntensity;
uniform float time;
uniform vec2 maskDirection;
uniform vec4 lineColor;
uniform vec2 lineStart;
uniform vec2 lineEnd;
uniform float weight;

float line(vec2 A, vec2 B, vec2 C, float thickness);

void main()
{
    // Calcula nuevas coordenadas de textura para simular el movimiento perpetuo de la máscara
    vec2 shiftedCoords = TexCoords + normalize(maskDirection) * maskSpeed * time;

    shiftedCoords.x = fract(shiftedCoords.x);
    shiftedCoords.y = fract(shiftedCoords.y);

    // Obtén el valor de la máscara en las nuevas coordenadas de textura
    float maskValue = texture(mask, shiftedCoords).r;

    // Ajusta la intensidad de la máscara
    maskValue *= maskIntensity;

    // Calcula el color final de la línea multiplicando el color de la máscara
    // por la mezcla original de la línea y el color de fondo
    FragColor = mix(vec4(0, 0, 0, 0), lineColor, line(lineStart, lineEnd, TexCoords, weight)) * maskValue;
}

float line(vec2 A, vec2 B, vec2 C, float thickness)
{
    vec2 AB = B - A;
    vec2 AC = C - A;

    float t = dot(AC, AB) / dot(AB, AB);
    t = min(1.0f, max(0.0f, t));

    vec2 Q = A + t * AB;

    float dist = length(Q - C);
    return smoothstep(0.003f, -dist, -thickness) + smoothstep(-0.004f, dist, thickness);
}