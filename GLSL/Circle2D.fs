#version 330 core

in vec2 TexCoords;
out vec4 FragColor;

uniform vec4  waveColor;
uniform float time;
uniform float numWaves;
uniform float speed;
uniform float thickness;

void main()
{
    vec2  uv   = TexCoords * 2.0 - 1.0;
    float dist = length(uv);
    if (dist > 1.0) discard;

    // Expanding rings: rings move outward as time increases
    float wave = fract(dist * numWaves - time * speed);

    // Smooth pulse at each ring boundary (where wave ≈ 0 or ≈ 1)
    float ring = 1.0 - smoothstep(0.0, thickness, min(wave, 1.0 - wave));

    // Fade out near the outer edge so rings vanish cleanly
    float fadeEdge = 1.0 - smoothstep(0.7, 1.0, dist);

    float alpha = ring * fadeEdge * waveColor.a;
    if (alpha < 0.01) discard;
    FragColor = vec4(waveColor.rgb, alpha);
}
