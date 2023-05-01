unsigned int createRGB(int r, int g, int b);

#define PI 3.14159
#define FLOW_SPEED_FACTOR 0.5
#define AMPLITUDE_FACTOR 3.0
#define NUMBER_OF_WAVES 20.0

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
    int i = get_global_id(0);

    int x = i % screenWidth;
    int y = i / screenWidth;

    float2 uv = { x, y };
    float2 resolution = { screenWidth, screenHeight};
    float2 st = uv / resolution;

    float2 center = { 0.5f, 0.5f };
    float2 offsetToCenter = {0.25f, 0.25f}; // center screen

    float2 offsetInput = { offsetX, offsetY };

    if (usingOffset > 0) {
        st /= 1.75f;
        st += center - offsetToCenter + offsetInput;
    }

    // -- WAVE EFFECT
    st.x += sin(st.y * NUMBER_OF_WAVES * PI + iTime * FLOW_SPEED_FACTOR) * (1.0 / resolution.x) * AMPLITUDE_FACTOR;

    int cx = st.x * screenWidth;
    int cy = st.y * screenHeight;
    int index = cy * screenWidth + cx;

    __global unsigned char *im = &image[index];

    float s = sin(iTime);
    float c = cos(iTime);
    float t = s * c;

    video[i] = createRGB(
        min(im[0] * (1 - s * 0.5f), 200.0f),
        min(im[1] * (1 - c * 0.5f), 200.0f),
        min(im[2] * (1 - t * 0.5f), 200.0f)
    );
}

unsigned int createRGB(int r, int g, int b)
{
    return (b << 16) + (g << 8) + (r);
}
