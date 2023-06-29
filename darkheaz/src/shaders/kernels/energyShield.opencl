#define CIRCLE_RADIUS 100
#define CIRCLE_THICKNESS 5
#define PI 3.14159265358979323846
#define RADIUS 0.07f
#define BRIGHTNESS 0.4f

unsigned int createRGB(int r, int g, int b);
unsigned int alphaBlend(unsigned int color1, unsigned int color2, unsigned int alpha);

__kernel void onUpdate(
    int screenWidth,
    int screenHeight,
    float time,
    __global unsigned int *video,
    __global bool *stencil,
    __global unsigned int *mask,
    unsigned int centerX,
    unsigned int centerY,
    unsigned int maxAlpha
)
{
    int i = get_global_id(0);

    int x = i % screenWidth;
    int y = i / screenWidth;

    float2 uv = { x, y };
    float2 center = {centerX, centerY};
    float dist = length(uv - center);
    float2 resolution = { (float)screenWidth, (float)screenHeight };
    float2 st = uv / resolution;

    if(stencil[i]) {
        float2 st_centered = st - 0.5f; // Center around (0.5, 0.5)
        float angle = time * 0.05f * 2 * PI; // Calculate rotation angle
        float2 nst;
        nst.x = st_centered.x * cos(angle) - st_centered.y * sin(angle);
        nst.y = st_centered.x * sin(angle) + st_centered.y * cos(angle);
        nst += 0.5f; // Re-center after rotation

        float intPart;
        nst.x = fract(nst.x, &intPart);
        nst.y = fract(nst.y, &intPart);

        int cx = nst.x * screenWidth;
        int cy = nst.y * screenHeight;
        int indexT = cy * screenWidth + cx;

        float alpha = 0.5f + 0.1f * sin(time * 100);
        unsigned int color = alphaBlend(video[i], mask[indexT], alpha * 255);

        alpha = 1.0f - clamp((dist - CIRCLE_RADIUS) / CIRCLE_THICKNESS, 0.0f, 1.0f);
        video[i] = alphaBlend(video[i], color, (unsigned int)(alpha * 255));
    }

    // Draw a circle with halo effect on top of everything
    float2 norm_uv = (uv - center) / resolution;
    norm_uv.x *= resolution.x / resolution.y;
    float d = length(norm_uv);

    float3 halo = BRIGHTNESS * (float3)(RADIUS / d);
    halo = mix(halo, (float3)(0.0f, 0.0f, 0.0f), d);
    halo *= (float3)(smoothstep(RADIUS, RADIUS + 0.01f, d));

    float3 color = (float3)(1.0f, 0.0f, 1.0f); // Adjust halo color here
    unsigned int haloColor = createRGB(color.x * 255, color.y * 255, color.z * 255);

    video[i] = alphaBlend(video[i], haloColor, halo.x * 255);
}

unsigned int createRGB(int r, int g, int b)
{
    return (b << 16) + (g << 8) + (r);
}

unsigned int alphaBlend(unsigned int color1, unsigned int color2, unsigned int alpha) {
    unsigned int rb = color1 & 0xff00ff;
    unsigned int g = color1 & 0x00ff00;
    rb += ((color2 & 0xff00ff) - rb) * alpha >> 8;
    g += ((color2 & 0x00ff00) - g) * alpha >> 8;

    return (rb & 0xff00ff) | (g & 0xff00);
}