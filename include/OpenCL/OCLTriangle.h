
typedef struct {
    int id;

    float A_x, A_y, A_z;
    float B_x, B_y, B_z;
    float C_x, C_y, C_z;

    float Ao_x, Ao_y, Ao_z;
    float Bo_x, Bo_y, Bo_z;
    float Co_x, Co_y, Co_z;

    float Ac_x, Ac_y, Ac_z;
    float Bc_x, Bc_y, Bc_z;
    float Cc_x, Cc_y, Cc_z;

    float An_x, An_y, An_z;
    float Bn_x, Bn_y, Bn_z;
    float Cn_x, Cn_y, Cn_z;

    float As_x, As_y;
    float Bs_x, Bs_y;
    float Cs_x, Cs_y;

    float o_x;
    float o_y;
    float o_z;

    float o_scale;

    float oPitch;
    float oYaw;
    float oRoll;

    int maxX, minX, maxY, minY;

    float normal[3];

    bool is_clipping;
} OCLTriangle;

struct OCLPlane {
    float A[3];
    float B[3];
    float C[3];
    float normal[3];
};

