#define PI 3.14159
#define FLOW_SPEED_FACTOR 0.5
#define AMPLITUDE_FACTOR 3.0
#define NUMBER_OF_WAVES 20.0

unsigned int createRGB(int r, int g, int b)
{
    return (b << 16) + (g << 8) + (r);
}

__kernel void onUpdate(
    int screenWidth,
    int screenHeight,
    float iTime,
    __global unsigned int *input,
    __global unsigned int *output
)
{
    const int x = get_global_id(0);
    const int y = get_global_id(1);
    const int i = y * screenWidth + x;

    if(x < screenWidth-1 && y < screenHeight-1) {
        unsigned int colorTL = input[y*screenWidth + x];
        unsigned int colorTR = input[y*screenWidth + x + 1];
        unsigned int colorBL = input[(y+1)*screenWidth + x];
        unsigned int colorBR = input[(y+1)*screenWidth + x + 1];

        const float fx = (float)(x % screenWidth) / screenWidth;
        const float fy = (float)(y % screenHeight) / screenHeight;

        const float bTL = (colorTL & 0xFF);
        const float bTR = (colorTR & 0xFF);
        const float bBL = (colorBL & 0xFF);
        const float bBR = (colorBR & 0xFF);

        const float gTL = ((colorTL >> 8) & 0xFF);
        const float gTR = ((colorTR >> 8) & 0xFF);
        const float gBL = ((colorBL >> 8) & 0xFF);
        const float gBR = ((colorBR >> 8) & 0xFF);

        const float rTL = ((colorTL >> 16) & 0xFF);
        const float rTR = ((colorTR >> 16) & 0xFF);
        const float rBL = ((colorBL >> 16) & 0xFF);
        const float rBR = ((colorBR >> 16) & 0xFF);

        const float b = bTL * (1 - fx) * (1 - fy) + bTR * fx * (1 - fy) + bBL * (1 - fx) * fy + bBR * fx * fy;
        const float g = gTL * (1 - fx) * (1 - fy) + gTR * fx * (1 - fy) + gBL * (1 - fx) * fy + gBR * fx * fy;
        const float r = rTL * (1 - fx) * (1 - fy) + rTR * fx * (1 - fy) + rBL * (1 - fx) * fy + rBR * fx * fy;

        output[i] = createRGB((int)b, (int)g, (int)r);
    } else {
        output[i] = input[i];
    }
}