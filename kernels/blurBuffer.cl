
unsigned int createRGBA(int r, int g, int b, int a);
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
    int i = get_global_id(0);

    int x = i % screenWidth;
    int y = i / screenWidth;

    // Efecto caja
    int rSum = 0, gSum = 0, bSum = 0, count = 0;
    for (int dy = -blurSize; dy <= blurSize; dy++) {
        for (int dx = -blurSize; dx <= blurSize; dx++) {
            int nx = clamp(x + dx, 0, screenWidth - 1);
            int ny = clamp(y + dy, 0, screenHeight - 1);

            unsigned int neighborColor = layer[ny * screenWidth + nx];
            rSum += neighborColor & 0xFF;
            gSum += (neighborColor >> 8) & 0xFF;
            bSum += (neighborColor >> 16) & 0xFF;
            count++;
        }
    }
    unsigned int avgR = rSum / count;
    unsigned int avgG = gSum / count;
    unsigned int avgB = bSum / count;
    unsigned int blurredColor = createRGBA(avgR, avgG, avgB, 255);


    // Mezcla el color difuminado con el color original del video
    unsigned int finalColor = alphaBlend(blurredColor, video[i], 200); // 127 es la alpha, ajusta según tus necesidades

    __global unsigned char *mi = (__global unsigned char *) &video[i];
    unsigned char *cc = (unsigned char *) &blurredColor;

    video[i] = createRGB(
        min((float)mi[0] + (float)cc[0], 255.0f),
        min((float)mi[1] + (float)cc[1], 255.0f),
        min((float)mi[2] + (float)cc[2], 255.0f)
    );

    // Hacer que los píxeles en 'layer' se desvanezcan
    unsigned int currentColor = layer[i];
    unsigned int r = currentColor & 0xFF;
    unsigned int g = (currentColor >> 8) & 0xFF;
    unsigned int b = (currentColor >> 16) & 0xFF;

    // Reduce la intensidad de los componentes RGB en un factor, como 1.1
    r = clamp((int)(r / 1.1), 0, 255);
    g = clamp((int)(g / 1.1), 0, 255);
    b = clamp((int)(b / 1.1), 0, 255);

    // Guarda el nuevo color en 'layer'
    layer[i] = createRGBA(r, g, b, 255);
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


unsigned int createRGB(int r, int g, int b)
{
    return (b << 16) + (g << 8) + (r);
}
