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
    int x = get_global_id(0);
    int y = get_global_id(1);
    int i = y * screenWidth + x;

    if(x < screenWidth-1 && y < screenHeight-1) {
        unsigned int colorTL = input[y*screenWidth + x];
        unsigned int colorTR = input[y*screenWidth + x + 1];
        unsigned int colorBL = input[(y+1)*screenWidth + x];
        unsigned int colorBR = input[(y+1)*screenWidth + x + 1];

        float fx = (float)(x % screenWidth) / screenWidth;
        float fy = (float)(y % screenHeight) / screenHeight;

        float bTL = (colorTL & 0xFF);
        float bTR = (colorTR & 0xFF);
        float bBL = (colorBL & 0xFF);
        float bBR = (colorBR & 0xFF);

        float gTL = ((colorTL >> 8) & 0xFF);
        float gTR = ((colorTR >> 8) & 0xFF);
        float gBL = ((colorBL >> 8) & 0xFF);
        float gBR = ((colorBR >> 8) & 0xFF);

        float rTL = ((colorTL >> 16) & 0xFF);
        float rTR = ((colorTR >> 16) & 0xFF);
        float rBL = ((colorBL >> 16) & 0xFF);
        float rBR = ((colorBR >> 16) & 0xFF);

        float b = bTL * (1 - fx) * (1 - fy) + bTR * fx * (1 - fy) + bBL * (1 - fx) * fy + bBR * fx * fy;
        float g = gTL * (1 - fx) * (1 - fy) + gTR * fx * (1 - fy) + gBL * (1 - fx) * fy + gBR * fx * fy;
        float r = rTL * (1 - fx) * (1 - fy) + rTR * fx * (1 - fy) + rBL * (1 - fx) * fy + rBR * fx * fy;

        output[i] = createRGB((int)b, (int)g, (int)r);
    } else {
        output[i] = input[i];
    }
}