unsigned int createRGB(int r, int g, int b);

__kernel void onUpdate(
    int screenWidth,
    int screenHeight,
    float iTime,
    __global unsigned int *video,
    float r,
    float g,
    float b,
    float progress
)
{
   const int i = get_global_id(0);

   __global unsigned char *c = &video[i];

   video[i] = createRGB(
       mix(c[0], (float) r, progress),
       mix(c[1], (float) g, progress),
       mix(c[2], (float) b, progress)
   );

}

unsigned int createRGB(int r, int g, int b)
{
    return (b << 16) + (g << 8) + (r);
}
