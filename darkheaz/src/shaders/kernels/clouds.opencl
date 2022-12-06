unsigned int createRGB(int r, int g, int b);
float plot(float, float, float);
float rand(float2 seed);
float noise(float2 st);
float line(float2 A, float2 B, float2 C, float thickness);

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

    float shadowIntensity = plot(st.x, 0.5, 0.8);
    float speed = 0.9;

    st *= 7.0;
    st.y = st.y + iTime * speed;

    float n = noise(st);
    int noiseIntensity = (int) (n * 75);

    int p1 = shadowIntensity * (t[0] - noiseIntensity);
    int p2 = shadowIntensity * (t[1] - noiseIntensity);
    int p3 = shadowIntensity * (t[2] - noiseIntensity);

    unsigned int color = createRGB(
        max(p1, 0),
        max(p2, 0),
        max(p3, 0)
    );

    video[i] = color;
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