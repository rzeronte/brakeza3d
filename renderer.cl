/* TYPES */
#define PI 3.141592
#define FRUSTUM_CLIPPING_DISTANCE 0.0001

typedef struct OCVertex3D {
    float x;
    float y;
    float z;
} OCVertex3D;

typedef struct OCPoint2D {
    int x;
    int y;
} OCPoint2D;

typedef struct Vector3D {
    OCVertex3D A;
    OCVertex3D B;
} Vector3D;

typedef struct OCTriangle {
    OCVertex3D A, B, C;
    OCVertex3D Ao, Bo, Co;
    OCVertex3D Ac, Bc, Cc;
    OCVertex3D An, Bn, Cn;
    OCPoint2D As, Bs, Cs;
    float fullArea;
    float reciprocalFullArea;
    OCVertex3D normal;
    float tex_u1_Ac_z, tex_u2_Bc_z, tex_u3_Cc_z;
    float tex_v1_Ac_z, tex_v2_Bc_z, tex_v3_Cc_z;
    float persp_correct_Az, persp_correct_Bz, persp_correct_Cz;
    int maxX, minX, maxY, minY;
    bool clipped;
} OCTriangle;

typedef struct OCLPlane {
    OCVertex3D A;
    OCVertex3D B;
    OCVertex3D C;
    OCVertex3D normal;
} OCLPlane;

typedef struct ObjectData {
    OCVertex3D position;  // Posición del objeto
    OCVertex3D rotation;  // Rotación del objeto
    float scale;        // Escala del objeto
} ObjectData;

typedef struct CameraData {
    OCVertex3D position;  // Posición de la cámara
    OCVertex3D rotation;  // Rotación de la cámara
} CameraData;

typedef struct FrustumData {
    OCVertex3D vNL;       // Vértice Near Plane Left
    OCVertex3D vNR;       // Vértice Near Plane Right
    OCVertex3D vNT;       // Vértice Near Plane Top
    OCVertex3D vNB;       // Vértice Near Plane Bottom
    OCLPlane planes[4];  // Frustum planes (OCPlanes)
} FrustumData;

typedef struct OCLMeshContext {
    ObjectData objectData;
    CameraData cameraData;
    FrustumData frustumData;
} OCLMeshContext;

/* FUNCTIONS */
int orient2d(OCPoint2D pa, OCPoint2D pb, int pc_x, int pc_y);
float processFullArea(OCPoint2D pb, OCPoint2D pc, OCPoint2D pa);
unsigned int createRGB(int r, int g, int b);
void renderTriangle(__global struct OCTriangle *t, int screenWidth, __global unsigned int *video, __global float *bufferDepth);
struct OCVertex3D objectSpace(struct OCVertex3D A, struct OCVertex3D objectPosition, struct OCVertex3D objectRotation, float oScale);
struct OCVertex3D cameraSpace(struct OCVertex3D V, struct OCVertex3D camPos, struct OCVertex3D camRot);
struct OCVertex3D NDCSpace(struct OCVertex3D v, struct OCVertex3D camPos, struct OCVertex3D camRot, struct OCVertex3D vNL, struct OCVertex3D vNR, struct OCVertex3D vNT, struct OCVertex3D vNB);
struct OCPoint2D screenSpace(struct OCVertex3D V, int screenWidth, int screenHeight);
struct OCVertex3D perspectiveDivision(struct OCVertex3D v, float frustumNearDist);
struct OCVertex3D rotateAxisX(struct OCVertex3D  V, float degrees);
struct OCVertex3D rotateAxisY(struct OCVertex3D  V, float degrees);
struct OCVertex3D rotateAxisZ(struct OCVertex3D  V, float degrees);
float degreesToRadians(float angle);
struct OCVertex3D subVertex(struct OCVertex3D v, float x, float y, float z);
struct OCVertex3D addVertex(struct OCVertex3D v, float x, float y, float z);
struct OCVertex3D scaleVertex(struct OCVertex3D v, float s);
bool testForClipping(__global struct OCLPlane *planes, OCVertex3D Ao, OCVertex3D Bo, OCVertex3D Co);
int isVector3DClippingPlane(struct OCLPlane plane, struct Vector3D V);
float distancePlaneVertex(struct OCLPlane plane, struct OCVertex3D p);
struct OCVertex3D getNormalize(struct OCVertex3D V);
struct OCVertex3D getNormal(__global struct OCTriangle *t);
struct OCVertex3D getComponent(struct Vector3D V);
struct OCVertex3D crossProduct(struct OCVertex3D v1, struct OCVertex3D v2);
void updateTriangle(__global struct OCTriangle *t, __global struct OCLMeshContext *context, int screenWidth, int screenHeight, __global unsigned int *video, __global float *bufferDepth);

/* KERNEL */

__kernel void onUpdate(
    int screenWidth,
    int screenHeight,
    __global unsigned int *videoBuffer,
    __global float *depthBuffer,
    __global OCLMeshContext *context,
    __global OCTriangle *triangles,
    int numTriangles
)
{
    int i = get_global_id(0);

    if (i < numTriangles) {
        updateTriangle(&triangles[i], context, screenWidth, screenHeight, videoBuffer, depthBuffer);
        //renderTriangle(&triangles[i], screenWidth, videoBuffer, depthBuffer);
    }
}

/***************************
* Functions
****************************/

void updateTriangle(__global struct OCTriangle *t, __global struct OCLMeshContext *context, int screenWidth, int screenHeight, __global unsigned int *video, __global float *bufferDepth)
{
    /*t->Ao = objectSpace(t->A, context->objectData.position, context->objectData.rotation, context->objectData.scale);
    t->Bo = objectSpace(t->B, context->objectData.position, context->objectData.rotation, context->objectData.scale);
    t->Co = objectSpace(t->C, context->objectData.position, context->objectData.rotation, context->objectData.scale);

    t->clipped = testForClipping(context->frustumData.planes, t->Ao, t->Bo, t->Co);

    t->Ac = cameraSpace(t->Ao, context->cameraData.position, context->cameraData.rotation);
    t->Bc = cameraSpace(t->Bo, context->cameraData.position, context->cameraData.rotation);
    t->Cc = cameraSpace(t->Co, context->cameraData.position, context->cameraData.rotation);

    t->An = NDCSpace(t->Ac, context->objectData.position, context->objectData.rotation, context->frustumData.vNL, context->frustumData.vNR, context->frustumData.vNT, context->frustumData.vNB);
    t->Bn = NDCSpace(t->Bc, context->objectData.position, context->objectData.rotation, context->frustumData.vNL, context->frustumData.vNR, context->frustumData.vNT, context->frustumData.vNB);
    t->Cn = NDCSpace(t->Cc, context->objectData.position, context->objectData.rotation, context->frustumData.vNL, context->frustumData.vNR, context->frustumData.vNT, context->frustumData.vNB);

    t->As = screenSpace(t->An, screenWidth, screenHeight);
    t->Bs = screenSpace(t->Bn, screenWidth, screenHeight);
    t->Cs = screenSpace(t->Cn, screenWidth, screenHeight);

    //video[t->As.y * screenWidth + t->As.x] = createRGB(255, 255, 255);
    //video[t->Bs.y * screenWidth + t->Bs.x] = createRGB(0, 255, 255);
    //video[t->Cs.y * screenWidth + t->Cs.x] = createRGB(255, 255, 0);

    t->normal = getNormal(t);
    */

    int A01 = (int) -( t->As.y - t->Bs.y );
    int A12 = (int) -( t->Bs.y - t->Cs.y );
    int A20 = (int) -( t->Cs.y - t->As.y );

    int B01 = (int) -( t->Bs.x - t->As.x );
    int B12 = (int) -( t->Cs.x - t->Bs.x );
    int B20 = (int) -( t->As.x - t->Cs.x );

    int w0_row = orient2d( t->Bs, t->Cs, t->minX, t->minY );
    int w1_row = orient2d( t->Cs, t->As, t->minX, t->minY );
    int w2_row = orient2d( t->As, t->Bs, t->minX, t->minY );

    float alpha, theta, gamma;

    float fullArea = processFullArea(t->Bs, t->Cs, t->As);
    float reciprocalFullArea = 1 / fullArea;

    for (int y = t->minY ; y < t->maxY ; y++) {

        int w0 = w0_row;
        int w1 = w1_row;
        int w2 = w2_row;

        for (int x = t->minX ; x < t->maxX ; x++) {
            if ((w0 | w1 | w2) > 0) {
                int bufferIndex = y * screenWidth + x;

                alpha = w0 * reciprocalFullArea;
                theta = w1 * reciprocalFullArea;
                gamma = 1 - alpha - theta;

                float depth = alpha * (t->An.z) + theta * (t->Bn.z) + gamma * (t->Cn.z);
                if (depth < bufferDepth[bufferIndex]) {
                    bufferDepth[bufferIndex] = depth;
                    video[bufferIndex] = createRGB(alpha * 255, theta * 255, gamma * 255);
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

void renderTriangle(__global struct OCTriangle *t, int screenWidth, __global unsigned int *video, __global float *bufferDepth)
{
    int A01 = (int) -( t->As.y - t->Bs.y );
    int A12 = (int) -( t->Bs.y - t->Cs.y );
    int A20 = (int) -( t->Bs.y - t->As.y );

    int B01 = (int) -( t->Bs.x - t->As.x );
    int B12 = (int) -( t->Cs.x - t->Bs.x );
    int B20 = (int) -( t->As.x - t->Cs.x );

    int maxX = max( t->As.x, max( t->Bs.x, t->Cs.x ) );
    int minX = min( t->As.x, min( t->Bs.x, t->Cs.x ) );
    int maxY = max( t->As.y, max( t->Bs.y, t->Cs.y ) );
    int minY = min( t->As.y, min( t->Bs.y, t->Cs.y ) );

    int w0_row = orient2d( t->Bs, t->Cs, minX, minY );
    int w1_row = orient2d( t->Cs, t->As, minX, minY );
    int w2_row = orient2d( t->As, t->Bs, minX, minY );

    float alpha, theta, gamma;

    float fullArea = processFullArea(t->Bs, t->Cs, t->As);
    float reciprocalFullArea = 1 / fullArea;

    for (int y = minY ; y < maxY ; y++) {

        int w0 = w0_row;
        int w1 = w1_row;
        int w2 = w2_row;

        for (int x = minX ; x < maxX ; x++) {

            if ((w0 | w1 | w2) > 0) {
                int bufferIndex = ( y * screenWidth ) + x;

                alpha =  (float) w0 * reciprocalFullArea;
                theta =  (float) w1 * reciprocalFullArea;
                gamma = 1 - alpha - theta;

                float depth = alpha * (t->An.z) + theta * (t->Bn.z) + gamma * (t->Cn.z);
                if (depth <= bufferDepth[bufferIndex]) {
                    bufferDepth[bufferIndex] = depth;
                    video[bufferIndex] = createRGB(alpha * 255, theta * 255, gamma * 255);
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

float processFullArea(OCPoint2D pb, OCPoint2D pc, OCPoint2D pa)
{
    return (float) orient2d(pb, pc, pa.x, pa.y);
}

int orient2d(OCPoint2D pa, OCPoint2D pb, int pc_x, int pc_y)
{
    return (pc_x - pa.x) * (pb.y - pa.y) - (pc_y - pa.y) * (pb.x - pa.x);
}

unsigned int createRGB(int r, int g, int b)
{
    return (b << 16) + (g << 8) + (r);
}

struct OCVertex3D objectSpace(struct OCVertex3D A, struct OCVertex3D objPos, struct OCVertex3D objRot, float oScale)
{
    A.x *= oScale;
    A.y *= oScale;
    A.z *= oScale;

    A = rotateAxisX(A, objRot.x);
    A = rotateAxisY(A, objRot.y);
    A = rotateAxisZ(A, objRot.z);

    A.x += objPos.x;
    A.y += objPos.y;
    A.z += objPos.z;

    return A;
}

struct OCVertex3D cameraSpace(struct OCVertex3D V, struct OCVertex3D camPos, struct OCVertex3D camRot)
{
    V.x -= camPos.x;
    V.y -= camPos.y;
    V.z -= camPos.z;

    V = rotateAxisX(V, camRot.x);
    V = rotateAxisY(V, camRot.y);
    V = rotateAxisZ(V, camRot.z);

    return V;
}

struct OCVertex3D NDCSpace(struct OCVertex3D v, struct OCVertex3D camPos, struct OCVertex3D camRot, struct OCVertex3D vNL, struct OCVertex3D vNR, struct OCVertex3D vNT, struct OCVertex3D vNB)
{
    struct OCVertex3D A;

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
}

struct OCPoint2D screenSpace(struct OCVertex3D V, int screenWidth, int screenHeight)
{
    struct OCPoint2D A;

    A.x = (1 + V.x) * ((float) screenWidth/2);
    A.y = (1 + V.y) * ((float) screenHeight/2);

    A.x = (int) A.x;
    A.y = (int) A.y;

    return A;
}

struct OCVertex3D perspectiveDivision(struct OCVertex3D V, float frustumNearDist)
{
    if (V.z != 0 || V.z != -0) {
        V.x =  - ( ( frustumNearDist * V.x) / V.z ) ;
        V.y =  - ( ( frustumNearDist * V.y) / V.z );
    }

    return V;
}

struct OCVertex3D rotateAxisX(struct OCVertex3D  V, float rads)
{
    struct OCVertex3D  A;

    A.x = 1 * V.x;
    A.y = cos(rads) * V.y - sin(rads) * V.z;
    A.z = sin(rads) * V.y + cos(rads) * V.z;

    return A;
}

struct OCVertex3D rotateAxisY(struct OCVertex3D V, float rads)
{
    struct OCVertex3D  A;

    A.x = ( cos(rads) * V.x ) + ( sin(rads) * V.z );
    A.y = 1 * V.y;
    A.z = ( -sin(rads) * V.x ) + ( cos(rads) * V.z );

    return A;
}

struct OCVertex3D rotateAxisZ(struct OCVertex3D V, float rads)
{
    struct OCVertex3D  A;

    A.x = ( cos(rads) * V.x ) - ( sin(rads) * V.y );
    A.y = ( sin(rads) * V.x ) + ( cos(rads) * V.y );
    A.z = 1 * V.z ;

    return A;
}

float degreesToRadians(float angle)
{
    return angle * PI / 180.0;
}

struct OCVertex3D subVertex(struct OCVertex3D v, float x, float y, float z)
{
    struct OCVertex3D vt;

    vt.x = v.x - x;
    vt.y = v.y - y;
    vt.z = v.z - z;

    return vt;
}

struct OCVertex3D addVertex(struct OCVertex3D v, float x, float y, float z)
{
    struct OCVertex3D vt;
    vt.x = v.x + x;
    vt.y = v.y + y;
    vt.z = v.z + z;

    return vt;
}

struct OCVertex3D scaleVertex(struct OCVertex3D v, float s)
{
    struct OCVertex3D vt;
    vt.x = v.x * s;
    vt.y = v.y * s;
    vt.z = v.z * s;

    return vt;
}

bool testForClipping(__global struct OCLPlane *planes, OCVertex3D Ao, OCVertex3D Bo, OCVertex3D Co)
{
    struct Vector3D edges[3];

    edges[0].A.x = Ao.x;
    edges[0].A.y = Ao.y;
    edges[0].A.z = Ao.z;
    edges[0].B.x = Bo.x;
    edges[0].B.y = Bo.y;
    edges[0].B.z = Bo.z;

    edges[1].A.x = Bo.x;
    edges[1].A.y = Bo.y;
    edges[1].A.z = Bo.z;
    edges[1].B.x = Co.x;
    edges[1].B.y = Co.y;
    edges[1].B.z = Co.z;

    edges[2].A.x = Co.x;
    edges[2].A.y = Co.y;
    edges[2].A.z = Co.z;
    edges[2].B.x = Ao.x;
    edges[2].B.y = Ao.y;
    edges[2].B.z = Ao.z;

    // 4 planes
    for (int i = 0 ; i < 4 ; i++) {
        // 3 vertices
        for (int e = 0 ; e < 3 ; e++) {
            if ( isVector3DClippingPlane( planes[i], edges[e] ) > 1 ) {
                return true;
            }
        }
    }

    return false;
}

int isVector3DClippingPlane(struct OCLPlane P, struct Vector3D V)
{
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

float distancePlaneVertex(struct OCLPlane plane, struct OCVertex3D p)
{
    float D = - ( (plane.normal.x * plane.A.x) + (plane.normal.y * plane.A.y) + (plane.normal.z * plane.A.z) );

    return ( (plane.normal.x * p.x) + (plane.normal.y * p.y) + (plane.normal.z * p.z) + D);
}

struct OCVertex3D getNormalize(struct OCVertex3D V)
{
    float modulo = fabs(sqrt( (V.x * V.x) + (V.y * V.y) + (V.z * V.z) ) );

    V.x /= modulo;
    V.y /= modulo;
    V.z /= modulo;

    return V;
}

struct OCVertex3D getNormal(__global struct OCTriangle *t)
{
    Vector3D vector1;
    vector1.A = t->Ao;
    vector1.B = t->Bo;

    Vector3D vector2;
    vector2.A = t->Ao;
    vector2.B = t->Co;

    return crossProduct( getComponent(vector1), getComponent(vector2) );
}

struct OCVertex3D getComponent(struct Vector3D V)
{
    struct OCVertex3D c;

    c.x = V.B.x - V.A.x;
    c.y = V.B.y - V.A.y;
    c.z = V.B.z - V.A.z;

    return c;
}

struct OCVertex3D crossProduct(struct OCVertex3D v1, struct OCVertex3D v2) {
    struct OCVertex3D r;

    r.x = (v1.y * v2.z) - (v1.z * v2.y);
    r.y = (v1.z * v2.x) - (v1.x * v2.z);
    r.z = (v1.x * v2.y) - (v1.y * v2.x);

    return r;
}