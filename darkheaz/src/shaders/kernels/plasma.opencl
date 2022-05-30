unsigned int createRGB(int r, int g, int b);
float plot(float2 st, float pct, float thickness);

__kernel void onUpdate(
    int screenWidth,
    int screenHeight,
    float iTime,
    __global unsigned int *palette,
    __global unsigned int *video
) {
   int i = get_global_id(0);

   int x = i % screenWidth;
   int y = i / screenWidth;

    //int intensity = 128.0 + (128.0 * sin(x / 8.0));
    int intensity = (
            128.0 + (128.0 * sin(x / 16.0))
            + 128.0 + (128.0 * sin(y / 32.0))
            + 128.0 + (128.0 * sin( sqrt((double)((x - screenWidth / 2.0)* (x - screenWidth / 2.0) + (y - screenHeight / 2.0) * (y - screenHeight / 2.0))) / 8.0))
            + 128.0 + (128.0 * sin(sqrt((double)(x * x + y * y)) / 8.0))
    ) / 4;

    int paletteShift = iTime * 100.0;
    float color = palette[(intensity + paletteShift) % 256];

    video[i] = createRGB( color, color, color);
}

unsigned int createRGB(int r, int g, int b)
{
    return (b << 16) + (g << 8) + (r);
}

float plot(float2 st, float pct, float thickness)
{
    return smoothstep(pct - thickness, pct, st[1]) - smoothstep(pct, pct + thickness, st[1]);
}
