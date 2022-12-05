unsigned int createRGB(int r, int g, int b);

__kernel void onUpdate(
    int screenWidth,
    int screenHeight,
    float iTime,
    __global unsigned int *video,
    __global unsigned int *shader,
    __global bool *stencil,
    __global unsigned int *buffer,
    float r,
    float g,
    float b
)
{
   int i = get_global_id(0);

   int x = i % screenWidth;
   int y = i / screenWidth;

   float2 uv = { x, y };
   float2 resolution = { screenWidth, screenHeight};
   float2 st = uv / resolution;

    __global unsigned char *s = &shader[i];
    __global unsigned char *z = &buffer[i];

    z[0] *= 0.85;
    z[1] *= 0.85;
    z[2] *= 0.85;

    if (stencil[i]) {
        buffer[i] = createRGB(r, g, b);
        video[i]  = shader[i];
        return;
    }

    video[i] = createRGB(
        s[0] + z[0],
        s[1] + z[1],
        s[2] + z[2]
    );

}

unsigned int createRGB(int r, int g, int b)
{
    return (b << 16) + (g << 8) + (r);
}