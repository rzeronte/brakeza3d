unsigned int createRGB(int r, int g, int b);

#define PI 3.14159f
#define FLOW_SPEED_FACTOR 0.5f
#define AMPLITUDE_FACTOR 3.0f
#define NUMBER_OF_WAVES 20.0f

float2 wrap(float2 uv, float2 resolution)
{
    return fmod(uv, resolution);
}

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

    float2 uv = { (float)x, (float)y };
    float2 resolution = { (float)screenWidth, (float)screenHeight };
    float2 st = uv / resolution;

    float2 center = { 0.5f, 0.5f };
    float2 offsetToCenter = { 0.25f, 0.25f }; // center screen

    float2 offsetInput = { offsetX, offsetY };

    if (usingOffset > 0) {
        st /= 1.75f;
        st += center - offsetToCenter + offsetInput;
    }

    // -- WAVE EFFECT
    st.x += sin(st.y * NUMBER_OF_WAVES * PI + iTime * FLOW_SPEED_FACTOR) * (1.0f / resolution.x) * AMPLITUDE_FACTOR;

    st = wrap(st, resolution);

    int cx = (int) (st.x * resolution.x);
    int cy = (int) (st.y * resolution.y);

    if (cx < 0) cx = 0;
    if (cx >= screenWidth) cx = screenWidth - 1;
    if (cy < 0) cy = 0;
    if (cy >= screenHeight) cy = screenHeight - 1;

    int index = cy * screenWidth + cx;

    __global unsigned char *im = &image[index];

    float s = sin(iTime);
    float c = cos(iTime);
    float t = s * c;

    video[i] = createRGB(
        min((int) (im[0] * (1.0f - s * 0.5f)), 200),
        min((int) (im[1] * (1.0f - c * 0.5f)), 200),
        min((int) (im[2] * (1.0f - t * 0.5f)), 200)
    );

}

unsigned int createRGB(int r, int g, int b)
{
    return (b << 16) + (g << 8) + (r);
}