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

    float2 uv = { (float) x, (float) y };
    float2 resolution = { (float) screenWidth, (float) screenHeight};
    float2 st = uv / resolution;


    unsigned int mixedColor = video[i];
    unsigned char *mi = &mixedColor;

    for (int j = 0; j < numberLasers; j++) {
        struct OCLaser laser = lasers[j];

        float2 A = { laser.x1, laser.y1 };
        float2 B = { laser.x2, laser.y2 };

        A = A / resolution;
        B = B / resolution;

        float l = line(A, B, st, 0.015f * laser.intensity);

        //--
        float rad  = 0.075f;
        float width = 0.2f;
        float len = length(A-st);
        float circleStart = smoothstep(rad-width, rad, len) - smoothstep(0, rad, len);

        len = length(B-st);
        float circleEnd = smoothstep(rad-width, rad, len) - smoothstep(0, rad, len);

        float3 colorStartCircle = {0.f, 0.f, 0.f};
        float3 colorEndCircle = {0.f, 0.f, 0.f};

        if (laser.startCircle) {
            colorStartCircle.x = circleStart * laser.r;
            colorStartCircle.y = circleStart * laser.g;
            colorStartCircle.z = circleStart * laser.b;
        }

        if (laser.endCircle) {
            colorEndCircle.x = circleEnd * laser.r;
            colorEndCircle.y = circleEnd * laser.g;
            colorEndCircle.z = circleEnd * laser.b;
        }

        //--

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

        float n = ci[0];

        float3 colorLine = {laser.r, laser.g, laser.b};
        colorLine = (colorLine/255.0f) * l * n;

        __global unsigned char *t = &video[i];

        colorStartCircle  = colorStartCircle/177.0f * n;
        colorEndCircle  = colorEndCircle/177.0f * n;

        mixedColor = createRGB(
            min(colorStartCircle.x + colorEndCircle.x + colorLine.x + mi[0], 255.0f),
            min(colorStartCircle.y + colorEndCircle.y + colorLine.y + mi[1], 255.0f),
            min(colorStartCircle.z + colorEndCircle.z + colorLine.z + mi[2], 255.0f)
        );
    }

    for (int j = 0; j < numberProjectiles; j++) {
        struct OCProjectile projectile = projectiles[j];
        float2 A = { projectile.x, projectile.y };

        A = A / resolution;

        float rad  = 0.0035f * projectile.intensity;
        float len = length(A-st);
        float width =  0.12f;
        float circle = smoothstep(rad-width, rad, len) - smoothstep(0, rad, len);

        __global unsigned char *ci = &image[i];

        float n = ci[0] * 1.5f;

        float3 colorCircle = {projectile.r, projectile.g, projectile.b};
        colorCircle = (colorCircle/255) * circle * n;

        mixedColor = createRGB(
            min(colorCircle.x + mi[0], 255.0f),
            min(colorCircle.y + mi[1], 255.0f),
            min(colorCircle.z + mi[2], 255.0f)
        );
    }

    video[i] = mixedColor;


}

unsigned int createRGB(int r, int g, int b)
{
    return (b << 16) + (g << 8) + (r);
}

float line(float2 A, float2 B, float2 C, float thickness)
 {
	float2 AB = B-A;
    float2 AC = C-A;

    float t = dot(AC, AB) / dot(AB, AB);
    t = min(1.0f, max(0.0f, t));

    float2 Q = A + t * AB;

    float dist = length(Q-C);
    return smoothstep(0.001f, -dist, -thickness) + smoothstep(-0.002f, dist, thickness);
}

