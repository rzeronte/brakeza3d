#version 330 core

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D sceneTexture;
uniform sampler2D depthTexture;

uniform float focusNear;
uniform float focusFar;
uniform float blurRadius;
uniform float far_plane;
uniform float intensity;

const float near_plane = 0.1;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0;
    return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));
}

float BlurFactor(float linearDepth)
{
    float b = 0.0;
    if (linearDepth < focusNear)
        b = (focusNear - linearDepth) / max(focusNear, 0.001);
    else if (linearDepth > focusFar)
        b = (linearDepth - focusFar) / max(far_plane - focusFar, 0.001);
    return clamp(b * intensity, 0.0, 1.0);
}

void main()
{
    ivec2 texSize = textureSize(sceneTexture, 0);
    vec2 invScreenSize = vec2(1.0 / float(texSize.x), 1.0 / float(texSize.y));

    float centerDepth  = LinearizeDepth(texture(depthTexture, TexCoords).r);
    float centerBlur   = BlurFactor(centerDepth);
    int   blurAmount   = clamp(int(centerBlur * blurRadius), 0, 6);

    vec4  colorSum  = vec4(0.0);
    float weightSum = 0.0;

    for (int dy = -blurAmount; dy <= blurAmount; ++dy) {
        for (int dx = -blurAmount; dx <= blurAmount; ++dx) {
            vec2  offset      = TexCoords + vec2(float(dx), float(dy)) * invScreenSize;
            float neighDepth  = LinearizeDepth(texture(depthTexture, offset).r);
            float depthDiff   = abs(centerDepth - neighDepth);
            float weight      = 1.0 / (1.0 + depthDiff * depthDiff * float(blurAmount));

            colorSum  += weight * texture(sceneTexture, offset);
            weightSum += weight;
        }
    }

    FragColor = weightSum > 0.0 ? colorSum / weightSum : texture(sceneTexture, TexCoords);
}
