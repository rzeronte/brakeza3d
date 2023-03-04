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
    bool startCircle;
    bool endCircle;
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
    unsigned char *mi = &mixedColor;


    for (int j = 0; j < numberLasers; j++) {
        struct OCLaser laser = lasers[j];

        float2 A = { laser.x1, laser.y1 };
        float2 B = { laser.x2, laser.y2 };

        A = A / resolution;
        B = B / resolution;

        float l = line(A, B, st, 0.015 * laser.intensity);

        //--
        float rad  = 0.075;
        float width = 0.2;
        float len = length(A-st);
        float circleStart = smoothstep(rad-width, rad, len) - smoothstep(0, rad, len);

        len = length(B-st);
        float circleEnd = smoothstep(rad-width, rad, len) - smoothstep(0, rad, len);

        float3 colorStartCircle = {0.f, 0.f, 0.f};
        float3 colorEndCircle = {0.f, 0.f, 0.f};

        if (laser.startCircle) {
            colorStartCircle[0] = circleStart * laser.r;
            colorStartCircle[1] = circleStart * laser.g;
            colorStartCircle[2] = circleStart * laser.b;
        }

        if (laser.endCircle) {
            colorEndCircle[0] = circleEnd * laser.r;
            colorEndCircle[1] = circleEnd * laser.g;
            colorEndCircle[2] = circleEnd * laser.b;
        }

        //--

        float2 nst = st;
        float2 offset = normalize(A-B) * iTime * 1.50;
        nst += offset;

        float intPart;
        nst.x = fract(nst.x, &intPart);
        nst.y = fract(nst.y, &intPart);

        int cx = nst.x * screenWidth;
        int cy = nst.y * screenHeight;
        int index = cy * screenWidth + cx;

        __global unsigned char *ci = &image[index];

        float n = ci[0];

        float3 colorLine = {laser.r, laser.g, laser.b};
        colorLine = (colorLine/255) * l * n;

        __global unsigned char *t = &video[i];

        colorStartCircle  = colorStartCircle/177 * n;
        colorEndCircle  = colorEndCircle/177 * n;

        mixedColor = createRGB(
            min(colorStartCircle[0] + colorEndCircle[0] + colorLine[0] + mi[0], 255.0),
            min(colorStartCircle[1] + colorEndCircle[1] + colorLine[1] + mi[1], 255.0),
            min(colorStartCircle[2] + colorEndCircle[2] + colorLine[2] + mi[2], 255.0)
        );
    }

    for (int j = 0; j < numberProjectiles; j++) {
        struct OCProjectile projectile = projectiles[j];
        float2 A = { projectile.x, projectile.y };

        A = A / resolution;

        float rad  = 0.0035 * projectile.intensity;
        float len = length(A-st);
        float width =  0.12;
        float circle = smoothstep(rad-width, rad, len) - smoothstep(0, rad, len);

        __global unsigned char *ci = &image[i];

        float n = ci[0] * 3;

        float3 colorCircle = {
            (circle * (projectile.r/255)),
            (circle * (projectile.g/255)),
            (circle * (projectile.b/255))
        };

        colorCircle *= n;

        mixedColor = createRGB(
            min(colorCircle[0] + mi[0], 255.0),
            min(colorCircle[1] + mi[1], 255.0),
            min(colorCircle[2] + mi[2], 255.0)
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

