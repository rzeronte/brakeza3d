struct OCLaser
{
    int x1;
    int y1;
    int x2;
    int y2;
    int r;
    int g;
    int b;
    float intensity;
};

struct OCProjectile
{
    int x;
    int y;
    int r;
    int g;
    int b;
    float intensity;
};

unsigned int createRGB(int r, int g, int b);
float plot(float, float, float);
float rand(float2 seed);
float line(float2 A, float2 B, float2 C, float thickness);

__kernel void onUpdate(
    int screenWidth,
    int screenHeight,
    float iTime,
    __global unsigned int *video,
    __global unsigned int *image,
    __global struct OCLaser *lasers,
    int numberLasers,
    __global struct OCProjectile *projectiles,
    int numberProjectiles
)
{
    int i = get_global_id(0);

    int x = i % screenWidth;
    int y = i / screenWidth;

    float2 uv = { x, y };
    float2 resolution = { screenWidth, screenHeight};
    float2 st = uv / resolution;

    unsigned int mixedColor = video[i];

    for (int j = 0; j < numberProjectiles; j++) {
        struct OCProjectile projectile = projectiles[j];
        float2 A = { projectile.x, projectile.y };

        A = A / resolution;

        float rad  = 0.0035 * projectile.intensity;
        float len = length(A-st);
        float width =  0.12;
        float circle = smoothstep(rad-width, rad, len) - smoothstep(0, rad, len);


        unsigned char *mi = &mixedColor;

        __global unsigned char *ci = &image[i];

        float n = ci[0] * 3;

        float3 colorCircle = {
            (circle * (projectile.r/255)) * n,
            (circle * (projectile.g/255)) * n,
            (circle * (projectile.b/255)) * n
        };
        mixedColor = createRGB(
            min(colorCircle[0] + mi[0], 255.0),
            min(colorCircle[1] + mi[1], 255.0),
            min(colorCircle[2] + mi[2], 255.0)
        );
    }

    for (int j = 0; j < numberLasers; j++) {
        struct OCLaser laser = lasers[j];

        float2 A = { laser.x1, laser.y1 };
        float2 B = { laser.x2, laser.y2 };

        float2 dir = normalize(A-B);

        A = A / resolution;
        B = B / resolution;

        float l = line(A, B, st, 0.015 * laser.intensity);

        float speed = 1.5;

        float2 nst = st;
        float2 offset = normalize(A-B) * iTime * speed;
        nst += offset;

        float intPart;
        nst.x = fract(nst.x, &intPart);
        nst.y = fract(nst.y, &intPart);

        int cx = nst.x * screenWidth;
        int cy = nst.y * screenHeight;
        int index = cy * screenWidth + cx;

        __global unsigned char *ci = &image[index];
        unsigned char *mi = &mixedColor;

        float n = ci[0];

        float3 colorLine = {
            (laser.r/255) * l * n,
            (laser.g/255) * l * n,
            (laser.b/255) * l * n
        };

        __global unsigned char *t = &video[i];

        mixedColor = createRGB(
            min(colorLine[0] + mi[0], 255.0),
            min(colorLine[1] + mi[1], 255.0),
            min(colorLine[2] + mi[2], 255.0)
        );
    }

    video[i] = mixedColor;
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

