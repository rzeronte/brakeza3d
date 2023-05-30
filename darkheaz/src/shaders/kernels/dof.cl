unsigned int createRGBA(int r, int g, int b, int a);
unsigned int alphaBlend(unsigned int color1, unsigned int color2, unsigned int alpha);
unsigned int calculateAlpha(int blurAmount, int numBlended);

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
    int i = get_global_id(0);

    int x = i % screenWidth;
    int y = i / screenWidth;

    unsigned int depthInt = bufferDepth[y * screenWidth + x];
    float depth = (float)(depthInt / 1000.0f) * intensity;

    float depthDifference = fabs(depth - focusPlaneDepth);

    int blurAmount = 0;
    if (depthDifference > focusRange) {
        blurAmount = (depthDifference - focusRange) * blurSize;
    }

    blurAmount = clamp(blurAmount, 0, 10);

    float colorSum[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    float weightSum = 0.0f;

    for (int dy = -blurAmount; dy <= blurAmount; dy++) {
        for (int dx = -blurAmount; dx <= blurAmount; dx++) {
            int nx = clamp(x + dx, 0, screenWidth - 1);
            int ny = clamp(y + dy, 0, screenHeight - 1);

            float thisDepth = (float)(bufferDepth[ny * screenWidth + nx] / 1000000.0f);
            float depthDifference = fabs(depth - thisDepth);
            float weight = 1.0f / (1.0f + depthDifference * depthDifference * blurSize);

            unsigned int color = video[ny * screenWidth + nx];
            unsigned int r = color & 0xFF;
            unsigned int g = (color >> 8) & 0xFF;
            unsigned int b = (color >> 16) & 0xFF;
            unsigned int a = (color >> 24) & 0xFF;
            colorSum[0] += weight * r;
            colorSum[1] += weight * g;
            colorSum[2] += weight * b;
            colorSum[3] += weight * a;
            weightSum += weight;
        }
    }

    if (weightSum > 0) {
        unsigned int blendedColor = createRGBA(colorSum[0] / weightSum, colorSum[1] / weightSum, colorSum[2] / weightSum, colorSum[3] / weightSum);
        video[y * screenWidth + x] = blendedColor;
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
unsigned int calculateAlpha(int blurAmount, int numBlended) {
    if (blurAmount == 0) {
        return 255;
    } else {
        float alpha = 255.0f / (blurAmount * 2 + 1);
        return (unsigned int)alpha;
    }
}