unsigned int createRGB(int r, int g, int b);

__kernel void onUpdate(
    int screenWidth,
    int screenHeight,
    __global unsigned int *video,
    __global bool *stencil,
    float r,
    float g,
    float b
)
{
    const int x = get_global_id(0);
    const int y = get_global_id(1);
    const int i = y * screenWidth + x;

    if (!stencil[i]) return;

    video[i] = createRGB((int) r, (int) g, (int) b);
}

unsigned int createRGB(int r, int g, int b)
{
    return (b << 16) + (g << 8) + (r);
}
