unsigned int createRGBA(int r, int g, int b, int a);
unsigned int alphaBlend(unsigned int color1, unsigned int color2, unsigned int alpha);

__kernel void onUpdate(
    int screenWidth,
    int screenHeight,
    float deltaTime,
    __global unsigned int *video,
    __global unsigned int *bufferDepth,
    float focusPlaneDepth,
    float focusRange,
    float blurSize,
    float intensity
)
{
    int x = get_global_id(0);
    int y = get_global_id(1);
    int i = y * screenWidth + x;

    const float depth = bufferDepth[i] * intensity;

    float depthDifference = fabs(depth - focusPlaneDepth);

    int blurAmount = 0;
    if (depthDifference > focusRange) {
        blurAmount = (depthDifference - focusRange) * blurSize;
    }
    blurAmount = clamp(blurAmount, 0, 10);

    float4 colorSum = {0.0f, 0.0f, 0.0f, 0.0f};
    float weightSum = 0.0f;

    for (int dy = -blurAmount; dy <= blurAmount; dy++) {
        for (int dx = -blurAmount; dx <= blurAmount; dx++) {
            int nx = clamp(x + dx, 0, screenWidth - 1);
            int ny = clamp(y + dy, 0, screenHeight - 1);

            const float thisDepth = (float)(bufferDepth[ny * screenWidth + nx] / 1000000.0f);
            const float depthDifference = fabs(depth - thisDepth);
            const float weight = 1.0f / (1.0f + depthDifference * depthDifference * blurSize);

            unsigned int color = video[ny * screenWidth + nx];
            unsigned int r = color & 0xFF;
            unsigned int g = (color >> 8) & 0xFF;
            unsigned int b = (color >> 16) & 0xFF;
            unsigned int a = (color >> 24) & 0xFF;
            colorSum.x += weight * r;
            colorSum.y += weight * g;
            colorSum.z += weight * b;
            colorSum.w += weight * a;
            weightSum += weight;
        }
    }

    colorSum = colorSum / weightSum;

    if (weightSum > 0) {
        video[i] = createRGBA(colorSum.x, colorSum.y, colorSum.z, colorSum.w);
    }
}

unsigned int createRGBA(int r, int g, int b, int a)
{
    return (a << 24) + (b << 16) + (g << 8) + (r);
}

unsigned int alphaBlend(unsigned int color1, unsigned int color2, unsigned int alpha) {
    unsigned int rb = color1 & 0xff00ff;
    unsigned int g = color1 & 0x00ff00;
    rb += ((color2 & 0xff00ff) - rb) * alpha >> 8;
    g += ((color2 & 0x00ff00) - g) * alpha >> 8;

    return (rb & 0xff00ff) | (g & 0xff00);
}
