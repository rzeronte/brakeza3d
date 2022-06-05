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

    /*
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
    */
       float2 fragCoord = { x, y };
       float2 resolution = { screenWidth, screenHeight};

        float2 uv = fragCoord/resolution;


        float grid = (float) fmod(floor(uv.x * 500.0f), 14.0f) < 0.5f;
        grid += fmod(floor(uv.y * 200.0f),11.0f) < 0.5f;
        grid = grid > 0.5f;

        float2 uvn = 2.0f * uv - 1.0f;

         grid = grid * 1.0f - clamp(0.0f, 1.0f, pow(length(uvn), 1.2f));
        // aquire wave
        float wa = sin(iTime/10);

        float3 COL1 = {0.0f, 0.0f, 0.0f};
        float3 COL2 = {0.0f, 0.0f, 0.0f};


        //attenuate
        float v = 1.0f - fabs(uv.y - wa);
        float e = pow(v, 20.0f);
        float3 t = { 0.0f, 1.0f, 0.0f };
        float3 t2 = {e, e, e};
        float3 col = t * grid + t2 * mix(COL1, COL2, e);

        col *= 127;

        video[i] = createRGB(
            col.x,
            col.y,
            col.z
        );
}

unsigned int createRGB(int r, int g, int b)
{
    return (b << 16) + (g << 8) + (r);
}

float plot(float2 st, float pct, float thickness)
{
    return smoothstep(pct - thickness, pct, st[1]) - smoothstep(pct, pct + thickness, st[1]);
}
