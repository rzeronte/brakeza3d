#version 330 core

in vec3 nearPoint;
in vec3 farPoint;

uniform mat4 model;
uniform mat4 projection;
uniform float gridSize;
uniform float gridOpacity;
uniform vec3 gridColor;
uniform vec3 cameraPos;
uniform float fadeStart;
uniform float fadeEnd;

out vec4 FragColor;

float computeDepth(vec3 pos) {
    vec4 clip = projection * model * vec4(pos, 1.0);
    return (clip.z / clip.w + 1.0) * 0.5;
}

void main() {
    float t = -nearPoint.y / (farPoint.y - nearPoint.y);
    if (t < 0.0) discard;

    vec3 fragPos3D = nearPoint + t * (farPoint - nearPoint);
    gl_FragDepth = computeDepth(fragPos3D);

    float dist = length(fragPos3D.xz - cameraPos.xz);
    float fade = (fadeEnd > fadeStart) ? 1.0 - smoothstep(fadeStart, fadeEnd, dist) : 1.0;
    if (fade <= 0.0) discard;

    // Grid lines
    vec2 coord = fragPos3D.xz * gridSize;
    vec2 gridFrac = abs(fract(coord - 0.5) - 0.5);
    vec2 derivative = fwidth(coord);
    vec2 gridLine = smoothstep(derivative * 0.5, derivative * 1.5, gridFrac);
    float line = 1.0 - min(gridLine.x, gridLine.y);

    // Axis lines with adaptive width
    vec2 axisWidth = fwidth(fragPos3D.xz) * 2.0;
    bool onZ = abs(fragPos3D.x) < axisWidth.x;
    bool onX = abs(fragPos3D.z) < axisWidth.y;

    if (onX) {
        FragColor = vec4(1.0, 0.0, 0.0, fade);
    } else if (onZ) {
        FragColor = vec4(0.0, 1.0, 0.0, fade);
    } else {
        FragColor = vec4(gridColor, line * gridOpacity * fade);
    }
}
