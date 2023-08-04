unsigned int createRGB(int r, int g, int b);
unsigned int alphaBlend(unsigned int color1, unsigned int color2, unsigned int alpha);
unsigned int lcg(unsigned int prev);
float getRandomFloat(float a, float b, unsigned int *randVal);
float pseudoRandom(int x);
float random(float x);

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

    unsigned int color = video[i];

    // Configura el espaciado y la altura de las barras
    int barSpacing = 150;
    int barHeight = 100;

    // Calcula la posición y el movimiento de las barras
    float rawBarPosition = y + time * 75 + sin(x * 0.01f + time);

    // Añade ruido
    rawBarPosition += pseudoRandom(x + y * screenWidth + (int)time) * 20.0f;

    // Aplica módulo después de añadir ruido
    int barPosition = (int)rawBarPosition % barSpacing;

    // Si la posición está dentro de la altura de la barra, pinta la barra de verde
    if (barPosition < barHeight && mask[i] > 0) {  // Se aplica el efecto solo donde la máscara es distinta de cero
        unsigned int greenColor = createRGB(0, 255, 0); // Color verde
        __global unsigned char *ci = &image[i];

        float alpha = ci[0]*0.25f;
        color = alphaBlend(color, greenColor, clamp((int)alpha, 0, (int) maxAlpha));
    }

    video[i] = color;
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

unsigned int lcg(unsigned int prev) {
    return (1103515245*prev + 12345) % 0x7fffffff;
}

float getRandomFloat(float a, float b, unsigned int *randVal) {

    *randVal = lcg(*randVal);
    return a + (*randVal / (float)0x7fffffff) * (b - a);
}

float pseudoRandom(int x)
{
    x = (x << 13) ^ x;
    return 1.0f - ((x * (x * x * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0f;
}

float random(float x)
{
    //repeat the image
    float intPart;
    float st = fract(sin(x) * 43758.5453123f, &intPart);

    return st;
}