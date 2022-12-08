unsigned int createRGB(int r, int g, int b);

__kernel void onUpdate(
    int screenWidth,
    int screenHeight,
    float iTime,
    __global unsigned int *video,
    __global unsigned int *shader,
    __global unsigned int *image,
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

    float2 center = { 0.5, 0.5 };
    float2 offset = {0.25, 0.25};

    float2 ieah = { offsetX, offsetY };

    st /= 1.75;
    st += center - offset + ieah;

    int cx = st.x * screenWidth;
    int cy = st.y * screenHeight;

    int index = cy * screenWidth + cx;

    video[i] = image[index];
}

unsigned int createRGB(int r, int g, int b)
{
    return (b << 16) + (g << 8) + (r);
}
