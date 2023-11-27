#define ALPHA_MIX 200
#define FALLOUT 0.9

unsigned int createRGB(int r, int g, int b);
unsigned int alphaBlend(unsigned int color1, unsigned int color2, unsigned int alpha);

__kernel void onUpdate(
    int screenWidth,
    int screenHeight,
    int blurSize,
    __global unsigned int *video,
    __global unsigned int *layer
)
{
    const int x = get_global_id(0);
    const int y = get_global_id(1);
    const int i = y * screenWidth + x;

    // Efecto caja
    int count = 0;
    uint3 rgbSum = {0, 0, 0};
    for (int dy = -blurSize; dy <= blurSize; dy++) {
        for (int dx = -blurSize; dx <= blurSize; dx++) {
            int nx = clamp(x + dx, 0, screenWidth - 1);
            int ny = clamp(y + dy, 0, screenHeight - 1);

            unsigned int neighborColor = layer[ny * screenWidth + nx];
            rgbSum.x += neighborColor & 0xFF;
            rgbSum.y += (neighborColor >> 8) & 0xFF;
            rgbSum.z += (neighborColor >> 16) & 0xFF;
            count++;
        }
    }

    rgbSum = rgbSum / count;

    uint blurredColor = createRGB(rgbSum.x, rgbSum.y, rgbSum.z);

    // Mezcla el color difuminado con el color original del video
    unsigned int finalColor = alphaBlend(blurredColor, video[i], ALPHA_MIX); // 127 es la alpha, ajusta según tus necesidades

    __global unsigned char *mi = (__global unsigned char *) &video[i];
    unsigned char *cc = (unsigned char *) &blurredColor;
    video[i] = createRGB(
        min(mi[0] + cc[0], 255),
        min(mi[1] + cc[1], 255),
        min(mi[2] + cc[2], 255)
    );

    // Hacer que los píxeles en 'layer' se desvanezcan
    unsigned int currentColor = layer[i];

    // Reduce la intensidad de los componentes RGB en un factor, como 1.1
    unsigned int r = clamp((int)((currentColor & 0xFF) * FALLOUT), 0, 255);
    unsigned int g = clamp((int)(((currentColor >> 8) & 0xFF) * FALLOUT), 0, 255);
    unsigned int b = clamp((int)(((currentColor >> 16) & 0xFF) * FALLOUT), 0, 255);

    // Guarda el nuevo color en 'layer'
    layer[i] = createRGB(r, g, b);
}

unsigned int alphaBlend(unsigned int color1, unsigned int color2, unsigned int alpha) {
    unsigned int rb = color1 & 0xff00ff;
    unsigned int g = color1 & 0x00ff00;
    rb += ((color2 & 0xff00ff) - rb) * alpha >> 8;
    g += ((color2 & 0x00ff00) - g) * alpha >> 8;

    return (rb & 0xff00ff) | (g & 0xff00);
}


unsigned int createRGB(int r, int g, int b)
{
    return (b << 16) + (g << 8) + (r);
}
