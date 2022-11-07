unsigned int createRGB(int r, int g, int b);
float plot(float2, float, float);
float rand (float2 seed);

__kernel void onUpdate(
    int screenWidth,
    int screenHeight,
    float iTime,
    __global unsigned int *video,
    __global unsigned int *videoShader
)
{
   int i = get_global_id(0);

   int x = i % screenWidth;
   int y = i / screenWidth;

   float2 uv = { x, y };
   float2 resolution = { screenWidth, screenHeight};
   float2 st = uv / resolution;

	float2 frag = st;
	frag *= 2.0 - 0.5 * cos (frag.xy) * sin(M_PI_F);
	frag *= 10.0;

    float random = rand(floor(frag));
	float2 black = smoothstep (1.0, 0.8, cos(frag * M_PI_F * 2.0));

	float3 color = {0, 0, 127};

    float intPart;
	float len = length(fract(frag, &intPart) - 0.5);

    color *= black[0] * black[1] * smoothstep (1.0, 0.0, len);
    color *= 0.5 + 0.5 * cos (random + random * iTime + iTime + 3.14159 * 0.5);

    int cx = st.x;
    int cy = st.y;
    int index = cy * screenWidth + cx;

    videoShader[i] = createRGB(
        color[0],
        color[1],
        color[2]
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

float rand (float2 seed)
{
    float2 p = {12.9898, 78.233};
    float intPart;

	return fract (sin (dot (seed, p)) * 137.5453, &intPart);
}