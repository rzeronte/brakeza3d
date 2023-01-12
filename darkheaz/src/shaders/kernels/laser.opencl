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
    __global unsigned int *image,
    int x1, int y1,
    int x2, int y2,
    float intensity,
    float r, float g, float b
)
{
   int i = get_global_id(0);

   int x = i % screenWidth;
   int y = i / screenWidth;

   float2 uv = { x, y };
   float2 resolution = { screenWidth, screenHeight};
   float2 st = uv / resolution;

    float2 A = { x1, y1 };
    float2 B = { x2, y2 };

    float2 dir = normalize(A-B);

    A = A / resolution;
    B = B / resolution;

    float l = line(A, B, st, 0.015 * intensity);

    float rad  = 0.0015;
    float len = length(B-st);
    float width = 0.1;
    float circle = smoothstep(rad-width, rad, len) - smoothstep(rad, rad+width, len);

    len = length(A-st);
    float circleStart = smoothstep(rad-width, rad, len) - smoothstep(rad, rad+width, len);

    float speed = 1.5;

    float2 offset = normalize(A-B) * iTime * speed;
    st += offset;

    float intPart;
	st.x = fract(st.x, &intPart);
	st.y = fract(st.y, &intPart);

    int cx = st.x * screenWidth;
    int cy = st.y * screenHeight;
    int index = cy * screenWidth + cx;

    __global unsigned char *ci = &image[index];

    float n = ci[0];

    float3 colorLine = {
        (r/255) * l * n,
        (g/255) * l * n,
        (b/255) * l * n
    };

    float3 colorCircle = { circle * 255, circle * 255, 0 };
    float3 colorCircleStart = { circleStart * 255, circleStart * 255, 0 };

    __global unsigned char *t = &video[i];

    video[i] = createRGB(
        min(colorCircleStart[0] + colorCircle[0] + colorLine[0] + t[0], 255.0),
        min(colorCircleStart[1] + colorCircle[1] + colorLine[1] + t[1], 255.0),
        min(colorCircleStart[2] + colorCircle[2] + colorLine[2] + t[2], 255.0)
    );
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
    return smoothstep(0.001, -dist, -thickness) + smoothstep(-0.002, dist, thickness);
}