#version 330 core

in vec3 nearPoint;
in vec3 farPoint;

uniform mat4 view;
uniform mat4 projection;
uniform float gridSize;
uniform float gridOpacity;
uniform vec3 gridColor;

out vec4 FragColor;

float computeDepth(vec3 pos) {
    vec4 clip_space_pos = projection * view * vec4(pos, 1.0);
    return (clip_space_pos.z / clip_space_pos.w);
}

vec4 grid(vec3 fragPos3D, float scale) {
    vec2 coord = fragPos3D.xz * scale;

    // Mejorar el antialiasing
    vec2 grid = abs(fract(coord - 0.5) - 0.5);
    vec2 derivative = fwidth(coord);
    vec2 gridLine = smoothstep(derivative * 0.5, derivative * 1.5, grid);

    float line = 1.0 - min(gridLine.x, gridLine.y);

    // Líneas de la rejilla
    vec4 color = vec4(gridColor, line);

    // Ejes siempre opacos (alpha = 1.0)
    if(abs(fragPos3D.x) < 0.1) {
        color = vec4(0.0, 1.0, 0.0, 1.0);
    }
    if(abs(fragPos3D.z) < 0.1) {
        color = vec4(1.0, 0.0, 0.0, 1.0);
    }

    return color;
}

void main() {
    float t = -nearPoint.y / (farPoint.y - nearPoint.y);

    if(t < 0.0) {
        discard;
    }

    vec3 fragPos3D = nearPoint + t * (farPoint - nearPoint);
    gl_FragDepth = computeDepth(fragPos3D);

    vec4 gridResult = grid(fragPos3D, gridSize);

    // Los ejes no se ven afectados por gridOpacity
    if(gridResult.rgb == vec3(0.0, 1.0, 0.0) || gridResult.rgb == vec3(1.0, 0.0, 0.0)) {
        FragColor = gridResult; // Alpha = 1.0 siempre
    } else {
        FragColor = vec4(gridResult.rgb, gridResult.a * gridOpacity);
    }
}