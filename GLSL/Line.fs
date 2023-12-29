#version 330 core

in vec2 TexCoords;

out vec4 FragColor;

uniform vec4 lineColor;
uniform vec2 lineStart;
uniform vec2 lineEnd;
uniform float weight;

float line(vec2 A, vec2 B, vec2 C, float thickness);

void main()
{
    FragColor = mix(vec4(0, 0, 0, 0), lineColor, line(lineStart, lineEnd, TexCoords, weight));
}

float line(vec2 A, vec2 B, vec2 C, float thickness)
{
    vec2 AB = B-A;
    vec2 AC = C-A;

    float t = dot(AC, AB) / dot(AB, AB);
    t = min(1.0f, max(0.0f, t));

    vec2 Q = A + t * AB;

    float dist = length(Q-C);
    return smoothstep(0.0001f, -dist, -thickness) + smoothstep(-0.0002f, dist, thickness);
}
