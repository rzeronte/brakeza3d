/* TYPES */
#define PI 3.141592
#define FRUSTUM_CLIPPING_DISTANCE 0.0001

typedef struct OCVertex3D {
    float x;
    float y;
    float z;
    float u;
    float v;
} OCVertex3D;

typedef struct OCLight {
    OCVertex3D position;
    OCVertex3D forward;
    float power;
    float kc;
    float kl;
    float kq;
    float specularComponent;
    unsigned int color;
    unsigned int colorSpecularity;
} OCLight;

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
    float w0_row, w1_row, w2_row;
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
    bool light;
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
    OCLPlane planes[6];  // Frustum planes (OCPlanes)
} FrustumData;

typedef struct OCLMeshContext {
    ObjectData objectData;
    CameraData cameraData;
    FrustumData frustumData;
} OCLMeshContext;

typedef struct OCFragment {
    int x;
    int y;

    float depth;

    float alpha;
    float gamma;
    float theta;

    float u;
    float v;
} OCFragment;

/* FUNCTIONS */
int orient2d(__global OCPoint2D *pa, __global OCPoint2D *pb, int pc_x, int pc_y);
float processFullArea(__global OCPoint2D *pb, __global OCPoint2D *pc, __global OCPoint2D *pa);
unsigned int createRGB(int r, int g, int b);
struct OCVertex3D objectSpace(__global struct OCVertex3D *A, __global struct OCVertex3D *objectPosition, __global struct OCVertex3D *objectRotation, float oScale);
struct OCVertex3D cameraSpace(__global struct OCVertex3D *V, __global struct OCVertex3D *camPos, __global struct OCVertex3D *camRot);
struct OCVertex3D NDCSpace(__global struct OCVertex3D *v, __global struct OCVertex3D *vNL, __global struct OCVertex3D *vNR, __global struct OCVertex3D *vNT, __global struct OCVertex3D *vNB);
struct OCPoint2D screenSpace(__global struct OCVertex3D *V, int screenWidth, int screenHeight);
void perspectiveDivision(struct OCVertex3D *v, float frustumNearDist);
struct OCVertex3D rotateAxisX(struct OCVertex3D *V, float rads);
struct OCVertex3D rotateAxisY(struct OCVertex3D *V, float rads);
struct OCVertex3D rotateAxisZ(struct OCVertex3D *V, float rads);
bool testForClipping(__global struct OCLPlane *planes, OCVertex3D Ao, OCVertex3D Bo, OCVertex3D Co);
int isVector3DClippingPlane(__global struct OCLPlane *plane, __private struct Vector3D *V);
float distancePlaneVertex(__global struct OCLPlane *plane, struct OCVertex3D p);
struct OCVertex3D getNormal(__global struct OCTriangle *t);
struct OCVertex3D getComponent(struct Vector3D *V);
struct OCVertex3D crossProduct(struct OCVertex3D v1, struct OCVertex3D v2);
float getXTextureFromUV(int textureWidth, float u);
float getYTextureFromUV(int textureHeight, float v);
void updateObjectSpace(__global struct OCTriangle *t, __global struct OCLMeshContext *context);
void updateNormal(__global struct OCTriangle *t);
void updateCameraSpace(__global struct OCTriangle *t, __global struct OCLMeshContext *context);
void updatePerspectiveNDCSpace(__global struct OCTriangle *t, __global struct OCLMeshContext *context);
void updateScreenSpace(__global struct OCTriangle *t, int screenWidth, int screenHeight);
void updateBoundingBox(__global struct OCTriangle *t);
void updateFullArea(__global struct OCTriangle *t);
void updateUVCache(__global struct OCTriangle *t);
float dotProduct(__private struct OCVertex3D *a, __private struct OCVertex3D *b);
bool isBackFaceCulling(__global struct OCTriangle *t, __global struct OCVertex3D *position);
bool isVertexInside(__global struct OCVertex3D *v, __global struct OCLPlane *planes);
unsigned int alphaBlend(unsigned int color1, unsigned int color2, unsigned int alpha);
unsigned int processPixelLights(__global struct OCTriangle *t, __global struct OCLight *lights, int numLights, unsigned int color, float alpha, float theta, float gamma);
unsigned int mixLightColor(__global struct OCLight *l, unsigned int color, __private struct OCVertex3D *Q, __global struct OCVertex3D *N);
float distanceBetweenVertices(__global struct OCVertex3D *v1, __private struct OCVertex3D *v2);
unsigned int mixColors(unsigned int color1, unsigned int color2, float t);
bool isPixelInWindow(int x, int y, int w, int h);

/* KERNEL */
__kernel void onUpdate(
    int screenWidth,
    int screenHeight,
    __global unsigned int *video,
    __global unsigned int *bufferDepth,
    __global unsigned int *texture,
    int surfaceWidth,
    int surfaceHeight,
    __global bool *stencil,
    int useStencil,
    __global OCLight *lights,
    int numLights,
    __global OCFragment *fragments
)
{
    int x = get_global_id(0);
    int y = get_global_id(1);
    int i = y * screenWidth + x;

    if (stencil[i]) {

        __global struct OCFragment *fragment = &fragments[i];

        bufferDepth[i] = fragment->depth;

        const int tx = (int) (surfaceWidth * fragment->u);
        const int ty = (int) (surfaceHeight * fragment->v);

        unsigned int color = texture[ty * surfaceWidth + tx];

        /*unsigned char *color_bytes = (unsigned char *)&color;
        unsigned char alphaChannel = color_bytes[3];
        if (alphaChannel < 255) {
            color = alphaBlend(video[i], color, alphaChannel);
        }*/

        video[i] = color;
    }
}

float processFullArea(__global OCPoint2D *pb, __global OCPoint2D *pc, __global OCPoint2D *pa)
{
    return (float) orient2d(pb, pc, pa->x, pa->y);
}

int orient2d(__global OCPoint2D *pa, __global OCPoint2D *pb, int pc_x, int pc_y)
{
    return (pc_x - pa->x) * (pb->y - pa->y) - (pc_y - pa->y) * (pb->x - pa->x);
}

unsigned int createRGB(int r, int g, int b)
{
    return (b << 16) + (g << 8) + (r);
}

struct OCVertex3D objectSpace(__global struct OCVertex3D *A, __global struct OCVertex3D *objectPosition, __global struct OCVertex3D *objectRotation, float oScale)
{
    OCVertex3D output = *A;

    output.x *= oScale;
    output.y *= oScale;
    output.z *= oScale;

    output = rotateAxisX(&output, objectRotation->x);
    output = rotateAxisY(&output, objectRotation->y);
    output = rotateAxisZ(&output, objectRotation->z);

    output.x += objectPosition->x;
    output.y += objectPosition->y;
    output.z += objectPosition->z;

    return output;
}

struct OCVertex3D cameraSpace(__global struct OCVertex3D *V, __global struct OCVertex3D *camPos, __global struct OCVertex3D *camRot)
{
    OCVertex3D output = *V;

    output.x -= camPos->x;
    output.y -= camPos->y;
    output.z -= camPos->z;

    output = rotateAxisX(&output, camRot->x);
    output = rotateAxisY(&output, camRot->y);
    output = rotateAxisZ(&output, camRot->z);

    return output;
}

struct OCVertex3D NDCSpace(__global struct OCVertex3D *v, __global struct OCVertex3D *vNL, __global struct OCVertex3D *vNR, __global struct OCVertex3D *vNT, __global struct OCVertex3D *vNB)
{
    OCVertex3D A = *v;

    perspectiveDivision(&A, 1);

    // NDC
    float Ax = A.x;                    // componente X de nuestro vértice en PANTALLA2D
    float vNLx = vNL->x;                // Límite Izquierdo de PANTALLA2D
    float vNRx = vNR->x;                // Límite Derecho de PANTALLA2D
    float tx0 = (Ax - vNLx);           // Distancia entre el límite Izquierdo y nuestro vértice
    float tx1 = 2 / ( vNRx - vNLx);    // Multiplicador (para 2 unidades, rango [0,2])
    float xt = (tx0 * tx1)  - 1;       // Calculamos el valor entre el rango [0,2], finalmente resta uno, tenemos [-1, 1]


    float Ay = A.y;
    float vNBy = vNB->y;
    float vNTy = vNT->y;
    float ty0 = (Ay - vNBy);
    float ty1 = 2 / (vNTy - vNBy);
    float yt = (ty0 * ty1)  - 1;

    A.x = xt;
    A.y = yt;

    return A;
}

struct OCPoint2D screenSpace(__global struct OCVertex3D *V, int screenWidth, int screenHeight)
{
    OCPoint2D A;

    A.x = (1 + V->x) * ((float) screenWidth/2);
    A.y = (1 + V->y) * ((float) screenHeight/2);

    return A;
}

void perspectiveDivision(struct OCVertex3D *V, float frustumNearDist)
{
    V->x = ( frustumNearDist * V->x) / V->z;
    V->y = ( frustumNearDist * V->y) / V->z;
}

struct OCVertex3D rotateAxisX(struct OCVertex3D *V, float rads)
{
    OCVertex3D  A;

    A.x = 1 * V->x;
    A.y = cos(rads) * V->y - sin(rads) * V->z;
    A.z = sin(rads) * V->y + cos(rads) * V->z;

    return A;
}

struct OCVertex3D rotateAxisY(struct OCVertex3D *V, float rads)
{
    OCVertex3D  A;

    A.x = ( cos(rads) * V->x ) + ( sin(rads) * V->z );
    A.y = 1 * V->y;
    A.z = ( -sin(rads) * V->x ) + ( cos(rads) * V->z );

    return A;
}

struct OCVertex3D rotateAxisZ(struct OCVertex3D *V, float rads)
{
    OCVertex3D  A;

    A.x = ( cos(rads) * V->x ) - ( sin(rads) * V->y );
    A.y = ( sin(rads) * V->x ) + ( cos(rads) * V->y );
    A.z = 1 * V->z ;

    return A;
}

bool testForClipping(__global struct OCLPlane *planes, OCVertex3D Ao, OCVertex3D Bo, OCVertex3D Co)
{
    Vector3D edges[3];

    edges[0].A = Ao;
    edges[0].B = Bo;

    edges[1].A = Bo;
    edges[1].B = Co;

    edges[2].A = Co;
    edges[2].B = Ao;

    // 4 planes
    for (int i = 0 ; i < 4 ; i++) {
        // 3 vertices
        for (int e = 0 ; e < 3 ; e++) {
            if ( isVector3DClippingPlane( &planes[i], &edges[e] ) > 1 ) {
                return true;
            }
        }
    }

    return false;
}

int isVector3DClippingPlane(__global struct OCLPlane *P, __private struct Vector3D *V)
{
    if (distancePlaneVertex(P, V->A) > FRUSTUM_CLIPPING_DISTANCE && distancePlaneVertex(P, V->B) > FRUSTUM_CLIPPING_DISTANCE) {
        return 1;
    }

    if (distancePlaneVertex(P, V->B) > FRUSTUM_CLIPPING_DISTANCE && distancePlaneVertex(P, V->A) < FRUSTUM_CLIPPING_DISTANCE) {
        return 2;
    }

    if (distancePlaneVertex(P, V->A) > FRUSTUM_CLIPPING_DISTANCE && distancePlaneVertex(P, V->B) < FRUSTUM_CLIPPING_DISTANCE) {
        return 3;
    }

    return 0;
}

float distancePlaneVertex(__global struct OCLPlane *plane, struct OCVertex3D p)
{
    float D = - ( (plane->normal.x * plane->A.x) + (plane->normal.y * plane->A.y) + (plane->normal.z * plane->A.z) );

    return ( (plane->normal.x * p.x) + (plane->normal.y * p.y) + (plane->normal.z * p.z) + D);
}

struct OCVertex3D getNormal(__global struct OCTriangle *t)
{
    Vector3D vector1;
    vector1.A = t->Ao;
    vector1.B = t->Bo;

    Vector3D vector2;
    vector2.A = t->Ao;
    vector2.B = t->Co;

    return crossProduct( getComponent(&vector1), getComponent(&vector2) );
}

struct OCVertex3D getComponent(struct Vector3D *V)
{
    OCVertex3D c;

    c.x = V->B.x - V->A.x;
    c.y = V->B.y - V->A.y;
    c.z = V->B.z - V->A.z;

    return c;
}

struct OCVertex3D crossProduct(struct OCVertex3D v1, struct OCVertex3D v2) {
    OCVertex3D r;

    r.x = (v1.y * v2.z) - (v1.z * v2.y);
    r.y = (v1.z * v2.x) - (v1.x * v2.z);
    r.z = (v1.x * v2.y) - (v1.y * v2.x);

    return r;
}

float getXTextureFromUV(int textureWidth, float u) {
    return (float) textureWidth * u;
}

float getYTextureFromUV(int textureHeight, float v) {
    return (float) textureHeight * v;
}

void updateObjectSpace(__global struct OCTriangle *t, __global struct OCLMeshContext *context)
{
    t->Ao = objectSpace(&t->A, &context->objectData.position, &context->objectData.rotation, context->objectData.scale);
    t->Bo = objectSpace(&t->B, &context->objectData.position, &context->objectData.rotation, context->objectData.scale);
    t->Co = objectSpace(&t->C, &context->objectData.position, &context->objectData.rotation, context->objectData.scale);
}

void updateNormal(__global struct OCTriangle *t) {
    t->normal = getNormal(t);
}

void updateCameraSpace(__global struct OCTriangle *t, __global struct OCLMeshContext *context) {
    t->Ac = cameraSpace(&t->Ao, &context->cameraData.position, &context->cameraData.rotation);
    t->Bc = cameraSpace(&t->Bo, &context->cameraData.position, &context->cameraData.rotation);
    t->Cc = cameraSpace(&t->Co, &context->cameraData.position, &context->cameraData.rotation);
}

void updatePerspectiveNDCSpace(__global struct OCTriangle *t, __global struct OCLMeshContext *context) {
    t->An = NDCSpace(&t->Ac, &context->frustumData.vNL, &context->frustumData.vNR, &context->frustumData.vNT, &context->frustumData.vNB);
    t->Bn = NDCSpace(&t->Bc, &context->frustumData.vNL, &context->frustumData.vNR, &context->frustumData.vNT, &context->frustumData.vNB);
    t->Cn = NDCSpace(&t->Cc, &context->frustumData.vNL, &context->frustumData.vNR, &context->frustumData.vNT, &context->frustumData.vNB);
}

void updateScreenSpace(__global struct OCTriangle *t, int screenWidth, int screenHeight) {
    t->As = screenSpace(&t->An, screenWidth, screenHeight);
    t->Bs = screenSpace(&t->Bn, screenWidth, screenHeight);
    t->Cs = screenSpace(&t->Cn, screenWidth, screenHeight);
}

void updateBoundingBox(__global struct OCTriangle *t) {
    t->maxX = max(t->As.x, max(t->Bs.x, t->Cs.x));
    t->minX = min(t->As.x, min(t->Bs.x, t->Cs.x));
    t->maxY = max(t->As.y, max(t->Bs.y, t->Cs.y));
    t->minY = min(t->As.y, min(t->Bs.y, t->Cs.y));
}

void updateFullArea(__global struct OCTriangle *t) {
    t->fullArea = processFullArea(&t->Bs, &t->Cs, &t->As);
}

void updateUVCache(__global struct OCTriangle *t) {
    t->tex_u1_Ac_z = t->A.u / t->Ac.z;
    t->tex_u2_Bc_z = t->B.u / t->Bc.z;
    t->tex_u3_Cc_z = t->C.u / t->Cc.z;

    t->tex_v1_Ac_z = t->A.v / t->Ac.z;
    t->tex_v2_Bc_z = t->B.v / t->Bc.z;
    t->tex_v3_Cc_z = t->C.v / t->Cc.z;

    t->persp_correct_Az = 1 / t->Ac.z;
    t->persp_correct_Bz = 1 / t->Bc.z;
    t->persp_correct_Cz = 1 / t->Cc.z;
}

// (v0 - P) . N
bool isBackFaceCulling(__global struct OCTriangle *t, __global struct OCVertex3D *position)
{
    OCVertex3D v;

    v.x = t->Ao.x - position->x;
    v.y = t->Ao.y - position->y;
    v.z = t->Ao.z - position->z;

    OCVertex3D normal = t->normal;
    return dotProduct(&v, &normal) >= 0;
}

float dotProduct(__private struct OCVertex3D *a, __private struct OCVertex3D *b) {
    return (a->x * b->x) + (a->y * b->y) + (a->z * b->z);
}

bool isVertexInside(__global struct OCVertex3D *v, __global struct OCLPlane *planes) {

    for (int i = 1; i <= 5; i++) {
        float distance = distancePlaneVertex(&planes[i], *v);

        if (distance  >= FRUSTUM_CLIPPING_DISTANCE) {
            return false;
        }
    }

    return true;
}

unsigned int alphaBlend(unsigned int color1, unsigned int color2, unsigned int alpha) {
    unsigned int rb = color1 & 0xff00ff;
    unsigned int g = color1 & 0x00ff00;
    rb += ((color2 & 0xff00ff) - rb) * alpha >> 8;
    g += ((color2 & 0x00ff00) - g) * alpha >> 8;

    return (rb & 0xff00ff) | (g & 0xff00);
}

unsigned int processPixelLights(__global struct OCTriangle *t, __global struct OCLight *lights, int numLights, unsigned int color, float alpha, float theta, float gamma)
{
    OCVertex3D D;
    D.x = alpha * t->Ao.x + theta * t->Bo.x + gamma * t->Co.x;
    D.y = alpha * t->Ao.y + theta * t->Bo.y + gamma * t->Co.y;
    D.z = alpha * t->Ao.z + theta * t->Bo.z + gamma * t->Co.z;

    for (int i = 0; i < numLights; i++) {
        color = mixLightColor(&lights[i], color, &D, &t->normal);
    }

    return color;
}

unsigned int mixLightColor(__global struct OCLight *l, unsigned int color, __private struct OCVertex3D *Q, __global struct OCVertex3D *N)
{
    float distance = distanceBetweenVertices(&l->position, Q);

    OCVertex3D R = l->forward;

    // Vector(Q, P)
    OCVertex3D component;
    component.x = l->position.x - Q->x;
    component.y = l->position.y - Q->y;
    component.z = l->position.z - Q->z;

    float modulo = sqrt((component.x * component.x) + (component.y * component.y) + (component.z * component.z));

    OCVertex3D L;
    L.x = component.x / modulo;
    L.y = component.y / modulo;
    L.z = component.z / modulo;
    //

    float C = pow(max(dotProduct(&R, &L), 0.0f), l->power) / (l->kc + l->kl * distance + l->kq * (distance * distance));

    unsigned int diffuseColor;
    if (C >= 1) {
        diffuseColor = l->color;
    } else {
        diffuseColor = mixColors(color, l->color, C);
    }

    return diffuseColor;
}

float distanceBetweenVertices(__global struct OCVertex3D *v1, __private struct OCVertex3D *v2)
{
    return sqrt( (v2->x - v1->x) * (v2->x - v1->x) + (v2->y - v1->y) * (v2->y - v1->y) + (v2->z - v1->z) * (v2->z - v1->z));
}

unsigned int mixColors(unsigned int color1, unsigned int color2, float t)
{
    unsigned char *c1 = (unsigned char *)&color1;
    unsigned char *c2 = (unsigned char *)&color2;

    return createRGB(
            mix((float) c1[0], (float) c2[0], t),
            mix((float) c1[1], (float) c2[1], t),
            mix((float) c1[2], (float) c2[2], t)
    );
}

bool isPixelInWindow(int x, int y, int w, int h) {
    return (x >= 0 && x < w && y >= 0 && y < h);
}