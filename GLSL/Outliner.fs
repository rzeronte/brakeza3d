#version 330 core

in vec2 texCoord;
out vec4 pixelColor;

uniform sampler2D image;
uniform vec3 lineColor;
uniform float borderThickness;

const float edgeThresholdSq = 0.36; // 0.6 * 0.6 — squared to avoid sqrt

void main() {
    vec2 step = vec2(borderThickness * 0.001);

    // Luminancia de los 8 vecinos (centro omitido: coef=0 en Sobel)
    float tl = dot(texture(image, texCoord + vec2(-1,-1) * step).rgb, vec3(0.333));
    float tc = dot(texture(image, texCoord + vec2( 0,-1) * step).rgb, vec3(0.333));
    float tr = dot(texture(image, texCoord + vec2( 1,-1) * step).rgb, vec3(0.333));
    float ml = dot(texture(image, texCoord + vec2(-1, 0) * step).rgb, vec3(0.333));
    float mr = dot(texture(image, texCoord + vec2( 1, 0) * step).rgb, vec3(0.333));
    float bl = dot(texture(image, texCoord + vec2(-1, 1) * step).rgb, vec3(0.333));
    float bc = dot(texture(image, texCoord + vec2( 0, 1) * step).rgb, vec3(0.333));
    float br = dot(texture(image, texCoord + vec2( 1, 1) * step).rgb, vec3(0.333));

    float edgeX = -tl - 2.0*ml - bl + tr + 2.0*mr + br;
    float edgeY = -tl - 2.0*tc - tr + bl + 2.0*bc + br;

    if (edgeX*edgeX + edgeY*edgeY > edgeThresholdSq) {
        pixelColor = vec4(lineColor, 1.0);
    } else {
        discard;
    }
}
