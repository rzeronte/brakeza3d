unsigned int createRGB(int r, int g, int b);

__kernel void onUpdate(
    int screenWidth,
    int screenHeight,
    float iTime,
    __global unsigned int *video
) {
    int i = get_global_id(0);
    int x = i % screenWidth;
    int y = i / screenWidth;

    float2 fragCoord = { x, y };
    float2 resolution = { screenWidth, screenHeight};

    float2 uv = fragCoord/resolution;
    float widthHeightRatio = resolution.x/resolution.y;
    uv.y/=widthHeightRatio;

    //Wave/////////////////////
    float slowing = 3.0;
    float frequency = 1.;
    float amplitudeLim = 2.;

    //Colors///
    float RGBTime = 2.;
    float colorWidth = 6.;

    float curvature = -pow(uv.x-0.5,2.)/0.5+0.5;
    float wave = cos((uv.x+iTime/slowing)*frequency)/amplitudeLim;
    float line = wave  *curvature+0.25;
    float2 centre = {uv.x,line};
    float2 pos = centre - uv;

    float dist = 1.0f/length(pos);
    dist *= 0.1f;

    dist = pow(dist, 0.8f);
    RGBTime *= iTime;

    float3 t = {sin(RGBTime+uv.x*colorWidth)/2.+0.5, -sin(RGBTime+uv.x*colorWidth)/2.+0.5, cos(RGBTime+uv.x*colorWidth)/2.+0.5};
    float3 col = dist * t ;
    col = 1.0f - exp( -col );

    videoShader[i] = createRGB(col.x * 255, col.y * 255, col.z * 255);
}

unsigned int createRGB(int r, int g, int b)
{
    return (b << 16) + (g << 8) + (r);
}
