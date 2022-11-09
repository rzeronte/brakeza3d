unsigned int createRGB(int r, int g, int b);
float plot(float2, float, float);
float rand(float2 seed);
float noise(float2 st);

__kernel void onUpdate(
    int screenWidth,
    int screenHeight,
    float iTime,
    __global unsigned int *video,
    __global unsigned int *shader,
    __global unsigned int *image
)
{
   int i = get_global_id(0);

   int x = i % screenWidth;
   int y = i / screenWidth;

   float2 uv = { x, y };
   float2 resolution = { screenWidth, screenHeight};
   float2 st = uv / resolution;

    int cx = uv.x;
    int cy = uv.y;
    int index = cy * screenWidth + cx;

    __global unsigned char *s = &image[index];
    __global unsigned char *t = &shader[i];
    float speed = 1.25;

    unsigned int color;

    int entero;
    st *= 10.0; // Scale the coordinate system by 10

    st.y = st.y + iTime * speed;

    // Use the noise function
    float n = noise(st);
    unsigned int output = createRGB(n * 255, n * 255 , n * 255);

    float2 fr = fract(n, &entero);  // get the fractional coords

    int test = (int) (n * 127);

    color = createRGB(
        max(t[0] - test, 0),
        max(t[1] - test, 0),
        max(t[2] - test, 0)
    );
    video[index] = color;
}

unsigned int createRGB(int r, int g, int b)
{
    return (b << 16) + (g << 8) + (r);
}

float plot(float2 st, float pct, float thickness)
{
    return smoothstep(pct - thickness, pct, st[0]) - smoothstep(pct, pct + thickness, st[0]);
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