unsigned int createRGB(int r, int g, int b);
unsigned int alphaBlend(unsigned int color1, unsigned int color2, unsigned int alpha);

__kernel void onUpdate(
    int screenWidth,
    int screenHeight,
    float time,
    __global unsigned int *video,
    __global unsigned int *image,
    __global unsigned int *mask,
    unsigned int maxAlpha
)
{
    int i = get_global_id(0);

    int x = i % screenWidth;
    int y = i / screenWidth;

    float2 uv = { x, y };
    float2 resolution = { screenWidth, screenHeight};
    float2 st = uv / resolution;

    float2 nst = st;
    float2 offset = {0, 1.0f};
    offset *= time * 0.05f;
    nst += offset;

    float intPart;
    nst.x = fract(nst.x, &intPart);
    nst.y = fract(nst.y, &intPart);

    int cx = nst.x * screenWidth;
    int cy = nst.y * screenHeight;
    int indexT = cy * screenWidth + cx;


    unsigned int imgPixel = image[indexT];
    unsigned int maskPixel = mask[i];
    unsigned int alpha = (maskPixel >> 24) & 0xFF;  // Suponemos que el canal alfa está en el byte más significativo.

    video[i] = alphaBlend(video[i], imgPixel, clamp((int)alpha, 0, (int) maxAlpha));  // Mezcla en base a la máscara
}

unsigned int createRGB(int r, int g, int b)
{
    return (b << 16) + (g << 8) + (r);
}

unsigned int alphaBlend(unsigned int color1, unsigned int color2, unsigned int alpha) {
    unsigned int rb = color1 & 0xff00ff;
    unsigned int g = color1 & 0x00ff00;
    rb += ((color2 & 0xff00ff) - rb) * alpha >> 8;
    g += ((color2 & 0x00ff00) - g) * alpha >> 8;

    return (rb & 0xff00ff) | (g & 0xff00);
}