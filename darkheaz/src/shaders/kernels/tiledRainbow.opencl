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

    float2 fragCoord = { x, y };
    float2 resolution = { screenWidth, screenHeight};

    float2 st  = fragCoord/resolution.y;
    float2 uv  = fragCoord/resolution.y;

    //these two lines create the tile pattern
    //scale the coordinate space by a certain number
    st*=8.0f;

    //repeat the image
    float intPart;
    st=fract(st, &intPart);

    //circle
    float2 p = {0.5f, 0.5f};
    float pct = fabs(distance(st, p) * 5.0f);

    //this line adds colour
    float3 t = {0,2,6};
    float3 col = 0.5f + 0.5f*cos(iTime*3.0f+uv.xyx+t);

    col=(col/pct);
    video[i] = createRGB(
        fmin(col.x * 255, 255),
        fmin(col.y * 255, 255),
        fmin(col.z * 255, 255)
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
