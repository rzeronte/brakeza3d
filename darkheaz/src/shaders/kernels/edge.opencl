unsigned int createRGB(int r, int g, int b);
float plot(float, float, float);
float rand(float2 seed);
float noise(float2 st);
float line(float2 A, float2 B, float2 C, float thickness);
float3 stencilColor(float2 uv, int screenWidth, __global bool *stencil);

__kernel void onUpdate(
    int screenWidth,
    int screenHeight,
    float iTime,
    __global unsigned int *video,
    __global unsigned int *shader,
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

    __global unsigned char *s = &shader[i];

    if (stencil[i]) {
        video[i] = shader[i];
    } else {
        video[i] = createRGB(
            (color[0] * r) + s[0],
            (color[1] * g) + s[1],
            (color[2] * b) + s[2]
        );
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

float plot(float st, float pct, float thickness)
{
    return smoothstep(pct - thickness, pct, st) - smoothstep(pct, pct + thickness, st);
}

float rand (float2 seed)
{
    float2 p = {12.9898, 78.233};
    float intPart;

	return fract (sin (dot (seed, p)) * 137.5453, &intPart);
}

float noise(float2 st) {
    float2 i = floor(st);

    int entero;
    float2 f = fract(st, &entero);

    // Four corners in 2D of a tile
    float a = rand(i);
    float2 left = {1.0, 0.0};
    float2 right = {0.0, 1.0};
    float2 bottomRight = {1.0, 1.0};

    float b = rand(i + left);
    float c = rand(i + right);
    float d = rand(i + bottomRight);

    // Smooth Interpolation
    float2 u = smoothstep(0.0, 1.0,f);

    // Mix 4 coorners percentages
    return mix(a, b, u.x) +
            (c - a) * u.y * (1.0 - u.x) +
            (d - b) * u.x * u.y
    ;
}

float line(float2 A, float2 B, float2 C, float thickness)
 {
	float2 AB = B-A;
    float2 AC = C-A;

    float t = dot(AC, AB) / dot(AB, AB);
    t = min(1.0, max(0.0, t));

    float2 Q = A + t * AB;

    float dist = length(Q-C);
    return smoothstep(-0.01, -dist, -thickness) + smoothstep(-0.02, dist, thickness);
}