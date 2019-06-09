struct OCLTriangle {
    float As_x, As_y;
    float Bs_x, Bs_y;
    float Cs_x, Cs_y;

    float An_x, An_y, An_z;
    float Bn_x, Bn_y, Bn_z;
    float Cn_x, Cn_y, Cn_z;

    float persp_correct_Az;
    float persp_correct_Bz;
    float persp_correct_Cz;

    float tex_u1_Ac_z;
    float tex_u2_Bc_z;
    float tex_u3_Cc_z;
    float tex_v1_Ac_z;
    float tex_v2_Bc_z;
    float tex_v3_Cc_z;

    float light_u1_Ac_z;
    float light_u2_Bc_z;
    float light_u3_Cc_z;
    float light_v1_Ac_z;
    float light_v2_Bc_z;
    float light_v3_Cc_z;

    bool isAnimated;
    bool isLightmapped;
};


int orient2d(int pa_x, int pa_y, int pb_x, int pb_y, int pc_x, int pc_y);
float processFullArea(int Bs_x, int Bs_y, int Cs_x, int Cs_y, int As_x, int As_y);
unsigned int createRGB(int r, int g, int b);

__kernel void rasterizer(__global struct OCLTriangle *tri, int SCREEN_WIDTH, __global unsigned int *video, __global float *ZBuffer)
{
    int i = get_global_id(0);

    int A01 = (int) -((int)tri[i].As_y - (int)tri[i].Bs_y);
    int A12 = (int) -((int)tri[i].Bs_y - (int)tri[i].Cs_y);
    int A20 = (int) -((int)tri[i].Cs_y - (int)tri[i].As_y);

    int B01 = (int) -((int)tri[i].Bs_x - (int)tri[i].As_x);
    int B12 = (int) -((int)tri[i].Cs_x - (int)tri[i].Bs_x);
    int B20 = (int) -((int)tri[i].As_x - (int)tri[i].Cs_x);

    int maxX = max(tri[i].As_x, max(tri[i].Bs_x, tri[i].Cs_x));
    int minX = min(tri[i].As_x, min(tri[i].Bs_x, tri[i].Cs_x));
    int maxY = max(tri[i].As_y, max(tri[i].Bs_y, tri[i].Cs_y));
    int minY = min(tri[i].As_y, min(tri[i].Bs_y, tri[i].Cs_y));

    int w0_row = orient2d(tri[i].Bs_x, tri[i].Bs_y, tri[i].Cs_x, tri[i].Cs_y, minX, minY);
    int w1_row = orient2d(tri[i].Cs_x, tri[i].Cs_y, tri[i].As_x, tri[i].As_y, minX, minY);
    int w2_row = orient2d(tri[i].As_x, tri[i].As_y, tri[i].Bs_x, tri[i].Bs_y, minX, minY);

    float alpha, theta, gamma;

    float fullArea = processFullArea(tri[i].Bs_x, tri[i].Bs_y, tri[i].Cs_x, tri[i].Cs_y, tri[i].As_x, tri[i].As_y);
    float reciprocalFullArea = 1 / fullArea;

    for (int y = minY ; y < maxY ; y++) {

            int w0 = w0_row;
            int w1 = w1_row;
            int w2 = w2_row;

            for (int x = minX ; x < maxX ; x++) {
                if ((w0 | w1 | w2) > 0) {
                    int bufferIndex = ( y * SCREEN_WIDTH ) + x;
                    alpha = w0 * reciprocalFullArea;
                    theta = w1 * reciprocalFullArea;
                    gamma = 1 - alpha - theta;

                    float depth = alpha * (tri[i].An_z) + theta * (tri[i].Bn_z) + gamma * (tri[i].Cn_z);

                    if (depth >= ZBuffer[bufferIndex] ) {
                        continue;
                    }

                    video[bufferIndex] = createRGB(alpha * 255, theta * 255, gamma * 255);
                    ZBuffer[bufferIndex] = depth;
                }

                // edge function increments
                w0 += A12;
                w1 += A20;
                w2 += A01;
            }

            w0_row += B12;
            w1_row += B20;
            w2_row += B01;
    }
}

float processFullArea(int Bs_x, int Bs_y, int Cs_x, int Cs_y, int As_x, int As_y)
{
    return orient2d(Bs_x, Bs_y, Cs_x, Cs_y, As_x, As_y);
}

int orient2d(int pa_x, int pa_y, int pb_x, int pb_y, int pc_x, int pc_y)
{
    return (pc_x - pa_x) * (pb_y - pa_y) - (pc_y - pa_y) * (pb_x - pa_x);
}

unsigned int createRGB(int r, int g, int b)
{
    return ((r & 0xff) << 16) + ((g & 0xff) << 8) + (b & 0xff);
}