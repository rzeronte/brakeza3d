struct OCShockWave {
    float iTime;
    float speed;
    int focalPointX;
    int focalPointY;
    float currentSize;
};

unsigned int createRGB(int r, int g, int b);

__kernel void onUpdate(
    int screenWidth,
    int screenHeight,
    float iTime,
    __global unsigned int *video,
    __global unsigned int *videoShader,
    __global struct OCShockWave *waves,
    int numberWaves

) {
    int i = get_global_id(0);
    int x = i % screenWidth;
    int y = i / screenWidth;

    float2 fragCoord = { x, y };

    unsigned int mixedColor = video[i];
    unsigned char *mi = &mixedColor;

    for (int j = 0; j < numberWaves; j++) {
        struct OCShockWave wave = waves[j];

        float2 focalPoint = { wave.focalPointX, wave.focalPointY };

        float magnification = -0.5f;

        float2 modifiedUV = fragCoord - focalPoint;

        float intPart;
        float edgecut = fract(wave.iTime * wave.speed, &intPart) * screenWidth;

        float res = smoothstep(
            edgecut - wave.currentSize,
            edgecut + wave.currentSize,
            length(modifiedUV)
        );
        float invertedRes = 1.0f - res;
        res = res * invertedRes * magnification;

        float2 resultUV = fragCoord - modifiedUV * res;

        int cx = resultUV.x;
        int cy = resultUV.y;
        int index = cy * screenWidth + cx;

        __global unsigned char *cc = &video[index];
        __global unsigned char *cs = &videoShader[i];

        mixedColor = createRGB(
            mix(cc[0], mi[0], 0.5),
            mix(cc[1], mi[1], 0.5),
            mix(cc[2], mi[2], 0.5)
        );
    }

    videoShader[i] = mixedColor;
}

unsigned int createRGB(int r, int g, int b)
{
    return (b << 16) + (g << 8) + (r);
}
