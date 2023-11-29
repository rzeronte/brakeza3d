unsigned int createRGB(int r, int g, int b);
unsigned int alphaBlend(unsigned int color1, unsigned int color2, unsigned int alpha);

#define PI 3.14159f
#define FLOW_SPEED_FACTOR 0.5f
#define AMPLITUDE_FACTOR 3.0f
#define NUMBER_OF_WAVES 20.0f

__kernel void onUpdate(
    int screenWidth,
    int screenHeight,
    float iTime,
    __global unsigned int *video,
    __global unsigned int *image,
    int usingOffset,
    float offsetX,
    float offsetY
)
{
    const int x = get_global_id(0);
    const int y = get_global_id(1);
    const int i = y * screenWidth + x;

    const float2 uv = { (float)x, (float)y };
    const float2 resolution = { (float)screenWidth, (float)screenHeight };
    float2 st = uv / resolution;

    if (usingOffset > 0) {
        const float2 center = { 0.5f, 0.5f };
        const float2 offsetToCenter = { 0.25f, 0.25f }; // center screen
        const float2 offsetInput = { offsetX, offsetY };

        st /= 1.75f;
        st += center - offsetToCenter + offsetInput;
    }

    // -- WAVE EFFECT
    st.x += sin(st.y * NUMBER_OF_WAVES * PI + iTime * FLOW_SPEED_FACTOR) * (1.0f / resolution.x) * AMPLITUDE_FACTOR;

    st = fmod(st, resolution);

    float2 cXY = st * resolution;

    const int index = (int) cXY.y * screenWidth + (int) cXY.x;

    unsigned int alpha = (image[index] >> 24) & 0xFF;

    video[i] = alphaBlend(video[i], image[index], alpha);
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