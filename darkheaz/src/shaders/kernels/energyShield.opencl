#define PI 3.14159265358979323846
#define RADIUS 0.08f
#define BRIGHTNESS 0.3f

#define MASK_WIDTH 128
#define MASK_HEIGHT 128

unsigned int createRGB(int r, int g, int b);
unsigned int alphaBlend(unsigned int color1, unsigned int color2, unsigned int alpha);

__kernel void onUpdate(
    int screenWidth,
    int screenHeight,
    float time,
    __global unsigned int *video,
    __global bool *stencil,
    __global unsigned int *texture,
    __global unsigned int *mask,
    unsigned int centerX,
    unsigned int centerY,
    unsigned int maxAlpha
)
{
    int i = get_global_id(0);

    int x = i % screenWidth;
    int y = i / screenWidth;

    float2 uv = { x, y };
    float2 center = {centerX, centerY};
    float dist = length(uv - center);
    float2 resolution = { (float)screenWidth, (float)screenHeight };
    float2 st = uv / resolution;

    int maskX = x - centerX + MASK_WIDTH / 2;
    int maskY = y - centerY + MASK_HEIGHT / 2;

    float rotationSpeed = 5.1;  // Controla la velocidad de rotación
    float angle = time * rotationSpeed;
    float s = sin(angle);
    float c = cos(angle);

    // Centramos las coordenadas alrededor del punto de rotación
    float2 rotationCenter = {centerX / (float)screenWidth, centerY / (float)screenHeight};
    float2 translatedSt = st - rotationCenter;

    // Aplicamos la rotación
    float2 rotatedCoords;
    rotatedCoords.x = c * translatedSt.x - s * translatedSt.y;
    rotatedCoords.y = s * translatedSt.x + c * translatedSt.y;

    // Deshacemos la translación
    rotatedCoords += rotationCenter;

    // Aseguramos que las coordenadas estén dentro de la textura
    rotatedCoords.x = fmod(rotatedCoords.x, 1.0f);
    rotatedCoords.y = fmod(rotatedCoords.y, 1.0f);

    // Obtenemos la posición de la textura
    int textureX = (int)(rotatedCoords.x * screenWidth);
    int textureY = (int)(rotatedCoords.y * screenHeight);

    if (maskX >= 0 && maskX < MASK_WIDTH && maskY >= 0 && maskY < MASK_HEIGHT) {
        unsigned int maskPixel = mask[maskY * MASK_WIDTH + maskX];
        unsigned int alpha = (maskPixel >> 24) & 0xFF;
        unsigned int textureColor = texture[textureY * screenWidth + textureX];
        video[i] = alphaBlend(video[i], textureColor, clamp((int)alpha, 0, (int) maxAlpha));
    }

        // Draw a circle with halo effect on top of everything
        float2 norm_uv = (uv - center) / resolution;
        norm_uv.x *= resolution.x / resolution.y;
        float d = length(norm_uv);

        float3 halo = BRIGHTNESS * (float3)(RADIUS / d);
        halo = mix(halo, (float3)(0.0f, 0.0f, 0.0f), d);
        halo *= (float3)(smoothstep(RADIUS, RADIUS + 0.01f, d));

        float3 color = (float3)(0.0f, 1.0f, 0.0f); // Adjust halo color here
        unsigned int haloColor = createRGB(color.x * 255, color.y * 255, color.z * 255);

        video[i] = alphaBlend(video[i], haloColor, halo.x * 255);
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