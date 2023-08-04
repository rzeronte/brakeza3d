unsigned int createRGB(int r, int g, int b);
float plot(float2 st, float pct, float thickness);

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

   float4 plotColor = {
        smoothstep(0, 1, st[0]),
        sin(st.x),
        pow(st.x, 2),
        0
   };

   float4 colorA = {255, 0, 0, 0};
   float4 colorB = {0, 0, 255, 0};

   float4 red   = {255, 0, 0, 255};
   float4 green = {0, 255, 0, 255};
   float4 blue  = {0, 0, 255, 255};

   float thickness = 0.1;

   float4 color = mix(colorA, colorB, st.x);
   color = mix(color, red, plot(st, plotColor[0], thickness));
   color = mix(color, green, plot(st, plotColor[1], thickness));
   color = mix(color, blue, plot(st, plotColor[2], thickness));

   video[i] = createRGB(color[0], color[1], color[2] );
}

unsigned int createRGB(int r, int g, int b)
{
    return (b << 16) + (g << 8) + (r);
}

float plot(float2 st, float pct, float thickness)
{
    return smoothstep(pct - thickness, pct, st[1]) - smoothstep(pct, pct + thickness, st[1]);
}
