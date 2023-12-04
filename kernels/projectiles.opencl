/* TYPES */
#define RADIUS_CIRCLE 0.025
#define WIDTH_CIRCLE 0.4

#define RADIUS_CIRCLE_LASER 0.0035
#define WIDTH_CIRCLE_LASER 0.12
#define AVERAGE_255 1/255
#define AVERAGE_177 1/77

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
float line(float2 A, float2 B, float2 C, float thickness);

void processProjectiles(int i, unsigned int *mixedColor, float2 st, int screenWidth, int screenHeight, __global struct OCProjectile *projectiles, int num, float iTime, __global unsigned int *video, __global unsigned int *image, float2 resolution);
void processLasers(int i, unsigned int *mixedColor, float2 st, int screenWidth, int screenHeight, __global struct OCLaser *lasers, int num, float iTime, __global unsigned int *video, __global unsigned int *image, float2 resolution);

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
    int x = get_global_id(0);
    int y = get_global_id(1);
    int i = y * screenWidth + x;

    const float2 uv = { (float) x, (float) y };
    const float2 resolution = { (float) screenWidth, (float) screenHeight};
    float2 st = uv / resolution;

    unsigned int mixedColor = video[i];

    processLasers(i, &mixedColor,  st, screenWidth,  screenHeight, lasers, numberLasers, iTime, video, image, resolution);
    //processProjectiles(i, &mixedColor,  st, screenWidth,  screenHeight, projectiles, numberProjectiles, iTime, video, image, resolution);

    video[i] = mixedColor;
}

unsigned int createRGB(int r, int g, int b)
{
    return (b << 16) + (g << 8) + (r);
}

float line(float2 A, float2 B, float2 C, float thickness)
{
    const float2 AB = B-A;
    const float2 AC = C-A;

    float t = dot(AC, AB) / dot(AB, AB);
    t = min(1.0f, max(0.0f, t));

    const float2 Q = A + t * AB;

    const float dist = length(Q-C);

    return smoothstep(0.001f, -dist, -thickness) + smoothstep(-0.002f, dist, thickness);
}

void processProjectiles(int i, unsigned int *mixedColor, float2 st,int screenWidth, int screenHeight, __global struct OCProjectile *projectiles, int num, float iTime, __global unsigned int *video, __global unsigned int *image, float2 resolution)
{
    unsigned char *mi = mixedColor;
    unsigned char *ci = &image[i];
    float2 invResolution = 1.0f / resolution;

    for (int j = 0; j < num; j++) {
        struct OCProjectile projectile = projectiles[j];
        float2 A = { projectile.x, projectile.y };

        A = A * invResolution;

        const float rad = RADIUS_CIRCLE_LASER * projectile.intensity;
        const float len = length(A - st);
        const float circle = smoothstep((float)(rad-WIDTH_CIRCLE_LASER), rad, len) - smoothstep(0, rad, len);

        const float n = ci[0] * 1.5f;

        float3 colorCircle = {projectile.r, projectile.g, projectile.b};
        colorCircle = (colorCircle/255) * circle * n;

        *mixedColor = createRGB(
            min(colorCircle.x + mi[0], 255.0f),
            min(colorCircle.y + mi[1], 255.0f),
            min(colorCircle.z + mi[2], 255.0f)
        );
    }
}

void processLasers(int i, unsigned int *mixedColor, float2 st, int screenWidth, int screenHeight, __global struct OCLaser *lasers, int num, float iTime, __global unsigned int *video, __global unsigned int *image, float2 resolution)
{
    unsigned char *mi = mixedColor;
    unsigned char *t = &video[i];

    const float2 invResolution = 1.0f / resolution;

    const float lineWeight = (RADIUS_CIRCLE - WIDTH_CIRCLE);

    for (int j = 0; j < num; j++) {
        struct OCLaser laser = lasers[j];

        float2 A = { laser.x1, laser.y1 };
        float2 B = { laser.x2, laser.y2 };

        A = A * invResolution;
        B = B * invResolution;

        float l = line(A, B, st, 0.015f * laser.intensity);

        float len = length(A - st);
        float circleStart = smoothstep(lineWeight, (float) RADIUS_CIRCLE, len) - smoothstep(0, (float) RADIUS_CIRCLE, len);

        len = length(B-st);
        float circleEnd = smoothstep(lineWeight, (float) RADIUS_CIRCLE, len) - smoothstep(0, (float) RADIUS_CIRCLE, len);

        float2 nst = st;
        float2 offset = normalize(A-B) * iTime * 1.50f;
        nst += offset;

        float intPart;
        nst.x = fract(nst.x, &intPart);
        nst.y = fract(nst.y, &intPart);

        int cx = nst.x * screenWidth;
        int cy = nst.y * screenHeight;
        int index = cy * screenWidth + cx;

        __global unsigned char *ci = &image[index];

        float3 colorLine = {laser.r, laser.g, laser.b};
        colorLine = colorLine * (float) AVERAGE_255 * l * ci[0];

        *mixedColor = createRGB(
            min(colorLine.x + mi[0], 255.0f),
            min(colorLine.y + mi[1], 255.0f),
            min(colorLine.z + mi[2], 255.0f)
        );
    }
}