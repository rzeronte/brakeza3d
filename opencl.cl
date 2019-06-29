/***************************
* Types
****************************/
struct OCLTriangle {
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

    float o_x, o_y, o_z;
    float o_scale;
    float oPitch, oYaw, oRoll;

    int maxX,  minX, maxY, minY;

    bool is_clipping;
};

struct OCLTile {
    bool draw;
    int id;
    int id_x;
    int id_y;
    int start_x;
    int start_y;
    int numTriangles;
    int triangleIds[1500];
};

struct Vertex3D {
    float x;
    float y;
    float z;
};

struct Point2D {
    int x;
    int y;
};

struct OCLPlane {
    float A[3];
    float B[3];
    float C[3];
    float normal[3];
};

struct Vector3D {
    struct Vertex3D A;
    struct Vertex3D B;
};

/***************************
* Predeclarations
****************************/
int orient2d(int pa_x, int pa_y, int pb_x, int pb_y, int pc_x, int pc_y);
float processFullArea(int Bs_x, int Bs_y, int Cs_x, int Cs_y, int As_x, int As_y);
unsigned int createRGB(int r, int g, int b);
void rasterizeTriangle(struct OCLTriangle t, int tileWidth, int tileHeight, int tileStartX, int tileStartY, int screenWidth, __global unsigned int *video, __global float *bufferDepth);
struct Vertex3D objectSpace(struct Vertex3D A, struct Vertex3D objectPosition, struct Vertex3D objectRotation, float oScale);
struct Vertex3D cameraSpace(struct Vertex3D V, struct Vertex3D camPos, struct Vertex3D camRot);
struct Vertex3D NDCSpace(struct Vertex3D v, struct Vertex3D camPos, struct Vertex3D camRot, struct Vertex3D vNL, struct Vertex3D vNR, struct Vertex3D vNT, struct Vertex3D vNB);
struct Point2D screenSpace(struct Vertex3D V, int screenWidth, int screenHeight);
struct Vertex3D perspectiveDivision(struct Vertex3D v, float frustumNearDist);
struct Vertex3D rotarEjeX(struct Vertex3D  V, float degrees);
struct Vertex3D rotarEjeY(struct Vertex3D  V, float degrees);
struct Vertex3D rotarEjeZ(struct Vertex3D  V, float degrees);
float degreesToRadians(float angle);
struct Vertex3D subVertex(struct Vertex3D v, float x, float y, float z);
struct Vertex3D addVertex(struct Vertex3D v, float x, float y, float z);
struct Vertex3D scaleVertex(struct Vertex3D v, float s);
bool testForClipping(
    __global struct OCLPlane *frustumPlanes,
    float Ao_x, float Ao_y, float Ao_z,
    float Bo_x, float Bo_y, float Bo_z,
    float Co_x, float Co_y, float Co_z
);
int isVector3DClippingPlane(struct OCLPlane plane, struct Vector3D V);
float distancePlaneVertex(struct OCLPlane plane, struct Vertex3D p);
struct Vertex3D getNormalize(struct Vertex3D V);

/***************************
* Kernels
****************************/
kernel __attribute__((reqd_work_group_size(1, 1, 1)))

__kernel void transformTrianglesKernel(
    __global struct OCLTriangle *triangles,
    float camX, float camY, float camZ,
    float yaw, float pitch, float roll,
    float vNL_x, float vNL_y, float vNL_z,
    float vNR_x, float vNR_y, float vNR_z,
    float vNT_x, float vNT_y, float vNT_z,
    float vNB_x, float vNB_y, float vNB_z,
    int screenWidth, int screenHeight,
    __global struct OCLPlane *frustumPlanes
) {
    int i = get_global_id(0);
    struct OCLTriangle t = triangles[i];

    struct Vertex3D A, B, C;
    struct Vertex3D Ao, Bo, Co;
    struct Vertex3D Ac, Bc, Cc;
    struct Vertex3D An, Bn, Cn;
    struct Point2D As, Bs, Cs;

    A.x = t.A_x;  A.y = t.A_y; A.z = t.A_z;
    B.x = t.B_x;  B.y = t.B_y; B.z = t.B_z;
    C.x = t.C_x;  C.y = t.C_y; C.z = t.C_z;

    struct Vertex3D objPos;
    objPos.x = t.o_x;
    objPos.y = t.o_y;
    objPos.z = t.o_z;

    struct  Vertex3D objRot;
    objRot.x = t.oPitch;
    objRot.y = t.oYaw;
    objRot.z = t.oRoll;

    float oScale = t.o_scale;

    struct Vertex3D camPos;
    camPos.x = camX;
    camPos.y = camY;
    camPos.z = camZ;

    struct  Vertex3D camRot;
    camRot.x = yaw;
    camRot.y = pitch;
    camRot.z = roll;

    struct Vertex3D vNL, vNR, vNT, vNB;
    vNL.x = vNL_x;     vNL.y = vNL_y;     vNL.z = vNL_z;
    vNR.x = vNR_x;     vNR.y = vNR_y;     vNR.z = vNR_z;
    vNT.x = vNT_x;     vNT.y = vNT_y;     vNT.z = vNT_z;
    vNB.x = vNB_x;     vNB.y = vNB_y;     vNB.z = vNB_z;

    //*******************

    Ao = objectSpace(A, objPos, objRot, oScale);
    Bo = objectSpace(B, objPos, objRot, oScale);
    Co = objectSpace(C, objPos, objRot, oScale);

    Ac = cameraSpace(Ao, camPos, camRot);
    Bc = cameraSpace(Bo, camPos, camRot);
    Cc = cameraSpace(Co, camPos, camRot);

    An = NDCSpace(Ac, objPos, camRot, vNL, vNR, vNT, vNB);
    Bn = NDCSpace(Bc, objPos, camRot, vNL, vNR, vNT, vNB);
    Cn = NDCSpace(Cc, objPos, camRot, vNL, vNR, vNT, vNB);

    As = screenSpace(An, screenWidth, screenHeight);
    Bs = screenSpace(Bn, screenWidth, screenHeight);
    Cs = screenSpace(Cn, screenWidth, screenHeight);

    //******************

    bool is_clipping = testForClipping(
        frustumPlanes,
        Ao.x, Ao.y, Ao.z,
        Bo.x, Bo.y, Bo.z,
        Co.x, Co.y, Co.z
    );

    //*******************
    
    triangles[i].is_clipping = is_clipping;

    triangles[i].maxX = max(t.As_x, max( t.Bs_x, t.Cs_x));
    triangles[i].minX = min(t.As_x, min( t.Bs_x, t.Cs_x));
    triangles[i].maxY = max(t.As_y, max( t.Bs_y, t.Cs_y));
    triangles[i].minY = min(t.As_y, min( t.Bs_y, t.Cs_y));

    triangles[i].Ao_x = Ao.x;
    triangles[i].Ao_y = Ao.y;
    triangles[i].Ao_z = Ao.z;

    triangles[i].Bo_x = Bo.x;
    triangles[i].Bo_y = Bo.y;
    triangles[i].Bo_z = Bo.z;

    triangles[i].Co_x = Co.x;
    triangles[i].Co_y = Co.y;
    triangles[i].Co_z = Co.z;

    triangles[i].Ac_x = Ac.x;
    triangles[i].Ac_y = Ac.y;
    triangles[i].Ac_z = Ac.z;

    triangles[i].Bc_x = Bc.x;
    triangles[i].Bc_y = Bc.y;
    triangles[i].Bc_z = Bc.z;

    triangles[i].Cc_x = Cc.x;
    triangles[i].Cc_y = Cc.y;
    triangles[i].Cc_z = Cc.z;

    triangles[i].An_x = An.x;
    triangles[i].An_y = An.y;
    triangles[i].An_z = An.z;

    triangles[i].Bn_x = Bn.x;
    triangles[i].Bn_y = Bn.y;
    triangles[i].Bn_z = Bn.z;

    triangles[i].Cn_x = Cn.x;
    triangles[i].Cn_y = Cn.y;
    triangles[i].Cn_z = Cn.z;

}

__kernel void rasterizerFrameTrianglesKernel(__global struct OCLTriangle *triangles, int SCREEN_WIDTH, __global unsigned int *video, __global float *ZBuffer)
{
    int i = get_global_id(0);

    int As_x = triangles[i].As_x;
    int Bs_x = triangles[i].Bs_x;
    int Cs_x = triangles[i].Cs_x;
    int As_y = triangles[i].As_y;
    int Bs_y = triangles[i].Bs_y;
    int Cs_y = triangles[i].Cs_y;

    int An_z = triangles[i].An_z;
    int Bn_z = triangles[i].Bn_z;
    int Cn_z = triangles[i].Cn_z;

    int A01 = (int) -( As_y - (int) Bs_y);
    int A12 = (int) -( Bs_y - (int) Cs_y);
    int A20 = (int) -( Cs_y - (int) As_y);

    int B01 = (int) -( Bs_x - (int) As_x);
    int B12 = (int) -( Cs_x - (int) Bs_x);
    int B20 = (int) -( As_x - (int) Cs_x);

    int maxX = max(As_x, max( Bs_x, Cs_x));
    int minX = min(As_x, min( Bs_x, Cs_x));
    int maxY = max(As_y, max( Bs_y, Cs_y));
    int minY = min(As_y, min( Bs_y, Cs_y));

    int w0_row = orient2d(Bs_x, Bs_y, Cs_x, Cs_y, minX, minY);
    int w1_row = orient2d(Cs_x, Cs_y, As_x, As_y, minX, minY);
    int w2_row = orient2d(As_x, As_y, Bs_x, Bs_y, minX, minY);

    float alpha, theta, gamma;

    float fullArea = processFullArea( Bs_x, Bs_y, Cs_x, Cs_y, As_x, As_y);
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
                        video[bufferIndex] = createRGB(alpha * 255, theta * 255, gamma * 255);
                        ZBuffer[bufferIndex] = depth;
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

__kernel void rasterizerFrameTileTrianglesKernel(
    int tileId,
    int screenWidth,
    int tileWidth,
    int tileHeight,
    __global unsigned int *bufferVideo,
    __global float *bufferDepth,
    __global struct OCLTriangle *triangles,
    __global struct OCLTile *tiles
) {
    int i = get_global_id(0);
    struct OCLTile tile = tiles[tileId];
    struct OCLTriangle triangle = triangles[i];

    rasterizeTriangle(triangle, tileWidth, tileHeight, tile.start_x, tile.start_y, screenWidth, bufferVideo, bufferDepth);
}

/***************************
* Types
****************************/

void rasterizeTriangle(struct OCLTriangle t, int tileWidth, int tileHeight, int tileStartX, int tileStartY, int screenWidth, __global unsigned int *video,  __global float *bufferDepth)
{
    int As_x = t.As_x;
    int Bs_x = t.Bs_x;
    int Cs_x = t.Cs_x;
    int As_y = t.As_y;
    int Bs_y = t.Bs_y;
    int Cs_y = t.Cs_y;

    int An_z = t.An_z;
    int Bn_z = t.Bn_z;
    int Cn_z = t.Cn_z;

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
                if  (! ((y < tileStartY || y > tileStartY + tileHeight) || (x < tileStartX || x > tileStartX + tileWidth)) ) {
                    int tx = x - (tileStartX);
                    int ty = y - (tileStartY);

                    int bufferIndex = ( ty * tileWidth ) + tx;

                    alpha = w0 * reciprocalFullArea;
                    theta = w1 * reciprocalFullArea;
                    gamma = 1 - alpha - theta;

                    float depth = alpha * (An_z) + theta * (Bn_z) + gamma * (Cn_z);
                    if (depth <= bufferDepth[bufferIndex]) {
                        bufferDepth[bufferIndex] = depth;
                        video[bufferIndex] = createRGB(alpha * 255, theta * 255, gamma * 255);
                    }
                }
            }
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

struct Vertex3D objectSpace(struct Vertex3D A, struct Vertex3D objPos, struct Vertex3D objRot, float oScale)
{
    struct Vertex3D v = A;

    v = scaleVertex(v, oScale);


    v = rotarEjeX(v, objRot.x);
    v = rotarEjeY(v, objRot.y);
    v = rotarEjeZ(v, objRot.z);

    v = addVertex(v, objPos.x, objPos.y, objPos.z);

    return v;
}

struct Vertex3D cameraSpace(struct Vertex3D V, struct Vertex3D camPos, struct Vertex3D camRot)
{
    struct Vertex3D A = V;

    A = subVertex(A, camPos.x, camPos.y, camPos.z);
    A = rotarEjeX(A, camRot.x);
    A = rotarEjeY(A, camRot.y);
    A = rotarEjeZ(A, camRot.z);

    return A;
}

struct Vertex3D NDCSpace(struct Vertex3D v, struct Vertex3D camPos, struct Vertex3D camRot, struct Vertex3D vNL, struct Vertex3D vNR, struct Vertex3D vNT, struct Vertex3D vNB)
{
    struct Vertex3D A;

    A = perspectiveDivision(v, 1);

    // NDC
    float Ax = A.x;                    // componente X de nuestro vértice en PANTALLA2D
    float vNLx = vNL.x;                // Límite Izquierdo de PANTALLA2D
    float vNRx = vNR.x;                // Límite Derecho de PANTALLA2D
    float tx0 = (Ax - vNLx);           // Distancia entre el límite Izquierdo y nuestro vértice
    float tx1 = 2 / ( vNRx - vNLx);    // Multiplicador (para 2 unidades, rango [0,2])
    float xt = (tx0 * tx1)  - 1;       // Calculamos el valor entre el rango [0,2], finalmente resta uno, tenemos [-1, 1]

    float Ay = A.y;
    float vNBy = vNB.y;
    float vNTy = vNT.y;
    float ty0 = (Ay - vNBy);
    float ty1 = 2 / (vNTy - vNBy);
    float yt = (ty0 * ty1)  - 1;

    A.x = xt;
    A.y = yt;

    A.z = v.z; // Almaceno z (deberia ser w)

    return A;
    return A;
}

struct Point2D screenSpace(struct Vertex3D V, int screenWidth, int screenHeight)
{
    struct Point2D A;

    A.x = (1 + V.x) * ((float) screenWidth/2);
    A.y = (1 + V.y) * ((float) screenHeight/2);

    A.x = (int) A.x;
    A.y = (int) A.y;

    return A;
}

struct Vertex3D perspectiveDivision(struct Vertex3D v, float frustumNearDist)
{
    struct Vertex3D A = v;

    if (v.z != 0 || v.z != -0) {
        A.x =  - ( ( frustumNearDist * v.x) / v.z ) ;
        A.y =  - ( ( frustumNearDist * v.y) / v.z );
    }

    return A;
}

struct Vertex3D rotarEjeX(struct Vertex3D  V, float rads)
{
    struct Vertex3D  A;

    A.x = 1 * V.x;
    A.y =  cos(rads) * V.y - sin(rads) * V.z;
    A.z =  sin(rads) * V.y + cos(rads) * V.z;

    return A;
}

struct Vertex3D rotarEjeY(struct Vertex3D  V, float rads)
{
    struct Vertex3D  A;

    A.x = ( cos(rads) * V.x ) + ( sin(rads) * V.z );
    A.y = 1 * V.y;
    A.z = ( -sin(rads) * V.x ) + ( cos(rads) * V.z );

    return A;
}

struct Vertex3D rotarEjeZ(struct Vertex3D  V, float rads)
{
    struct Vertex3D  A;

    A.x = ( cos(rads) * V.x ) - ( sin(rads) * V.y );
    A.y = ( sin(rads) * V.x ) + ( cos(rads) * V.y );
    A.z = 1 * V.z ;

    return A;
}

float degreesToRadians(float angle)
{
    return angle * (float) M_PI / (float) 180.0;;
}

struct Vertex3D subVertex(struct Vertex3D v, float x, float y, float z)
{
    struct Vertex3D vt;

    vt.x = v.x - x;
    vt.y = v.y - y;
    vt.z = v.z - z;

    return vt;
}

struct Vertex3D addVertex(struct Vertex3D v, float x, float y, float z)
{
    struct Vertex3D vt;
    vt.x = v.x + x;
    vt.y = v.y + y;
    vt.z = v.z + z;

    return vt;
}

struct Vertex3D scaleVertex(struct Vertex3D v, float s)
{
    struct Vertex3D vt;
    vt.x = v.x * s;
    vt.y = v.y * s;
    vt.z = v.z * s;

    return vt;
}

bool testForClipping(
    __global struct OCLPlane *planes,
    float Ao_x, float Ao_y, float Ao_z,
    float Bo_x, float Bo_y, float Bo_z,
    float Co_x, float Co_y, float Co_z
) {
    struct Vector3D edges[3];

    edges[0].A.x = Ao_x;
    edges[0].A.y = Ao_y;
    edges[0].A.z = Ao_z;
    edges[0].B.x = Bo_x;
    edges[0].B.y = Bo_y;
    edges[0].B.z = Bo_z;

    edges[1].A.x = Bo_x;
    edges[1].A.y = Bo_y;
    edges[1].A.z = Bo_z;
    edges[1].B.x = Co_x;
    edges[1].B.y = Co_y;
    edges[1].B.z = Co_z;

    edges[2].A.x = Co_x;
    edges[2].A.y = Co_y;
    edges[2].A.z = Co_z;
    edges[2].B.x = Ao_x;
    edges[2].B.y = Ao_y;
    edges[2].B.z = Ao_z;

     // 4 planes
    for (int i = 0 ; i <= 3 ; i++) {
        // 3 vertices
        for (int e = 0 ; e < 3 ; e++) {
            if ( isVector3DClippingPlane(planes[i], edges[e]) > 1 ) {
                return true;
            }
        }
    }

    return false;
}

int isVector3DClippingPlane(struct OCLPlane P, struct Vector3D V)
{
    float FRUSTUM_CLIPPING_DISTANCE = 0.0001f;

    if (distancePlaneVertex(P, V.A) > FRUSTUM_CLIPPING_DISTANCE && distancePlaneVertex(P, V.B) > FRUSTUM_CLIPPING_DISTANCE) {
        return 1;
    }

    if (distancePlaneVertex(P, V.B) > FRUSTUM_CLIPPING_DISTANCE && distancePlaneVertex(P, V.A) < FRUSTUM_CLIPPING_DISTANCE) {
        return 2;
    }

    if (distancePlaneVertex(P, V.A) > FRUSTUM_CLIPPING_DISTANCE && distancePlaneVertex(P, V.B) < FRUSTUM_CLIPPING_DISTANCE) {
        return 3;
    }

    return 0;
}

float distancePlaneVertex(struct OCLPlane plane, struct Vertex3D p)
{
    struct Vertex3D n;

    n.x = plane.normal[0];
    n.y = plane.normal[1];
    n.z = plane.normal[2];

    //n = getgetNormalize(n);

    float D = - ( (n.x * plane.A[0]) + (n.y * plane.A[1]) + (n.z * plane.A[2]) );
    float dist = ( (n.x * p.x) + (n.y * p.y) + (n.z * p.z) + D);

    return dist;
}

struct Vertex3D getNormalize(struct Vertex3D V)
{
    float modulo = fabs(sqrt( (V.x * V.x) + (V.y * V.y) + (V.z * V.z) ) );

    struct Vertex3D C;

    C.x = V.x / modulo;
    C.y = V.y / modulo;
    C.z = V.z / modulo;

    return C;
}