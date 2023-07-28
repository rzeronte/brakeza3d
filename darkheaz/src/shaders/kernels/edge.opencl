unsigned int createRGB(int r, int g, int b);
float3 stencilColor(float2 uv, int screenWidth, __global bool *stencil);

__kernel void onUpdate(
    int screenWidth,
    int screenHeight,
    float iTime,
    __global unsigned int *video,
    __global bool *stencil,
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


    float size = 1.0;

    float2 dML = {size, 0.0};
    float2 dMR = {0.0, size};

    float2 dBL = {size, 0.0};
    float2 dBM = {0.0, size};
    float2 dBR = {size, size};

    float2 dTL = {-size, -size};
    float2 dTM = {0.0, -size};
    float2 dTR = {-size, size};

    float3 ML = stencilColor( uv + dML, screenWidth, stencil);
    float3 MR = stencilColor( uv + dMR, screenWidth, stencil);

    float3 BL = stencilColor( uv + dBL, screenWidth, stencil);
    float3 BM = stencilColor( uv + dBM, screenWidth, stencil);
    float3 BR = stencilColor( uv + dBR, screenWidth, stencil);

    float3 TL = stencilColor( uv + dTL, screenWidth, stencil);
    float3 TM = stencilColor( uv + dTM, screenWidth, stencil);
    float3 TR = stencilColor( uv + dTR, screenWidth, stencil);

    float3 GradX = -TL + TR - ML + MR - BL + BR;
    float3 GradY = TL + TM + TR - BL - BM - BR;

    float2 c1 = {GradX[0], GradY[0]};
    float2 c2 = {GradX[1], GradY[1]};
    float2 c3 = {GradX[2], GradY[2]};

    float l1 = length(c1);
    float l2 = length(c2);
    float l3 = length(c3);

    float3 color = { l1, l2, l3 };

    if (stencil[i]) {
        video[i] = video[i];
    } else {
        if (l1 + l2 + l3 > 0) {
            video[i] = createRGB(r, g, b);
        } else {
            video[i] = video[i];
        }
    }
}

float3 stencilColor(float2 uv, int screenWidth, __global bool *stencil)
{
    int cx = uv.x;
    int cy = uv.y;
    int index = cy * screenWidth + cx;

    float3 result;

    if (stencil[index]) {
        result = createRGB(255, 255, 255);
    } else {
        result = createRGB(0, 0, 0);
    }

    return result;
}

unsigned int createRGB(int r, int g, int b)
{
    return (b << 16) + (g << 8) + (r);
}
