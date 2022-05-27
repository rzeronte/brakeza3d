unsigned int createRGB(int r, int g, int b);

__kernel void onUpdate(
    int screenWidth,
    int screenHeight,
    __global unsigned int *video)
{
   int i = get_global_id(0);

   int x = i % screenWidth;
   int y = i / screenWidth;

   float2 uv = { x, y };
   float2 resolution = { screenWidth, screenHeight};
   float2 st = uv / resolution;

   video[i] = createRGB( 0, 0, 0 );
}

unsigned int createRGB(int r, int g, int b)
{
    return (b << 16) + (g << 8) + (r);
}
