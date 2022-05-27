unsigned int createRGB(int r, int g, int b);

__kernel void onUpdate(
    int screenWidth,
    int screenHeight,
    float iTime,
    float speed,
    int focalPointX,
    int focalPointY,
    float currentSize,
    __global unsigned int *video,
    __global unsigned int *videoShader
) {
    int i = get_global_id(0);
    int x = i % screenWidth;
    int y = i / screenWidth;

    float2 fragCoord = { x, y };
    float2 focalPoint = { focalPointX, focalPointY };

    float magnification = -0.5f;

    float2 modifiedUV = fragCoord - focalPoint;

    float intPart;
    float edgecut = fract(iTime*speed, &intPart) * screenWidth;

    float res = smoothstep(
        edgecut - currentSize,
        edgecut + currentSize,
        length(modifiedUV)
    );
    float invertedRes = 1.0f - res;
    res = res * invertedRes * magnification;

    float2 resultUV = fragCoord - modifiedUV * res;

    int cx = resultUV.x;
    int cy = resultUV.y;
    int index = cy * screenWidth + cx;
    videoShader[i] = video[index];
}

unsigned int createRGB(int r, int g, int b)
{
    return (b << 16) + (g << 8) + (r);
}
