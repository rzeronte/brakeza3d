#version 330 core

in vec2 TexCoords;
out vec4 FragColor;

uniform vec4  lineColor;
uniform vec2  lineStart;    // normalized [0,1]: x=horizontal, y=vertical
uniform vec2  lineEnd;
uniform float weight;       // full thickness in pixels
uniform vec2  resolution;   // render width, height

void main()
{
    vec2 px = TexCoords * resolution;
    vec2 A  = lineStart * resolution;
    vec2 B  = lineEnd   * resolution;

    vec2 AB = B - A;
    float t = clamp(dot(px - A, AB) / dot(AB, AB), 0.0, 1.0);
    float dist = length(px - (A + t * AB));

    float halfW = weight * 0.5;
    float alpha = lineColor.a * (1.0 - smoothstep(halfW - 0.5, halfW + 0.5, dist));
    FragColor = vec4(lineColor.rgb, alpha);
}
