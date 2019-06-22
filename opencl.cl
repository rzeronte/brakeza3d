struct OCLTriangle
 {
    float As_x, As_y;
    float Bs_x, Bs_y;
    float Cs_x, Cs_y;

    float An_z;
    float Bn_z;
    float Cn_z;
};

struct pixelFragment {
    int x;
    int y;
    int depth;
    int triangleId;
    int w0;
    int w1;
    int w2;
};


int orient2d(int pa_x, int pa_y, int pb_x, int pb_y, int pc_x, int pc_y);
float processFullArea(int Bs_x, int Bs_y, int Cs_x, int Cs_y, int As_x, int As_y);
unsigned int createRGB(int r, int g, int b);

kernel __attribute__((reqd_work_group_size(1, 1, 1)))

__kernel void rasterizer( int SCREEN_WIDTH, __global unsigned int *video, __global float *ZBuffer, __global struct OCLTriangle *tri)
{
    int i = get_global_id(0);

    int As_x = tri[i].As_x;
    int Bs_x = tri[i].Bs_x;
    int Cs_x = tri[i].Cs_x;
    int As_y = tri[i].As_y;
    int Bs_y = tri[i].Bs_y;
    int Cs_y = tri[i].Cs_y;

    int An_z = tri[i].An_z;
    int Bn_z = tri[i].Bn_z;
    int Cn_z = tri[i].Cn_z;

    int A01 = (int) -( As_y - Bs_y );
    int A12 = (int) -( Bs_y - Cs_y );
    int A20 = (int) -( Cs_y - As_y );

    int B01 = (int) -( Bs_x - As_x );
    int B12 = (int) -( Cs_x - Bs_x );
    int B20 = (int) -( As_x - Cs_x );

    int maxX = max( As_x, max( Bs_x, Cs_x ) );
    int minX = min( As_x, min( Bs_x, Cs_x ) );
    int maxY = max( As_y, max( Bs_y, Cs_y ) );
    int minY = min( As_y, min( Bs_y, Cs_y ) );

    int w0_row = orient2d( Bs_x, Bs_y, Cs_x, Cs_y, minX, minY );
    int w1_row = orient2d( Cs_x, Cs_y, As_x, As_y, minX, minY );
    int w2_row = orient2d( As_x, As_y, Bs_x, Bs_y, minX, minY );

    float alpha, theta, gamma;

    float fullArea = processFullArea(Bs_x, Bs_y, Cs_x, Cs_y, As_x, As_y);
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

                    float depth = alpha * (An_z) + theta * (Bn_z) + gamma * (Cn_z);

                    if (depth <= ZBuffer[bufferIndex] ) {
                        ZBuffer[bufferIndex] = depth;
                        video[bufferIndex] = createRGB(alpha * 255, theta * 255, gamma * 255);
                    }

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