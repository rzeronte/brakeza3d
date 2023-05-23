typedef struct OCVertex3D {
    float x;
    float y;
    float z;
    float u;
    float v;
} OCVertex3D;

typedef struct OCLPlane {
    OCVertex3D A;
    OCVertex3D B;
    OCVertex3D C;
    OCVertex3D normal;
} OCLPlane;

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

typedef struct OCSwarmContext {
    float separationWeight;
    float alignmentWeight;
    float cohesionWeight;
    float distanceNeighbor;
    float maxVelocity;
    CameraData cameraData;
    FrustumData frustumData;
} OCSwarmContext;

typedef struct OCBoid {
    OCVertex3D position;
    OCVertex3D velocity;
    OCVertex3D color;
} OCBoid;

typedef struct OCPoint2D {
    int x;
    int y;
} OCPoint2D;

unsigned int createRGB(int r, int g, int b);
unsigned int lcg(unsigned int prev);
float getRandomFloat(float a, float b, unsigned int *randVal);
unsigned int alphaBlend(unsigned int color1, unsigned int color2, unsigned int alpha);
struct OCVertex3D objectSpace(struct OCVertex3D *A, struct OCVertex3D *objectPosition, struct OCVertex3D *objectRotation, float oScale);
struct OCVertex3D cameraSpace(struct OCVertex3D *V, struct OCVertex3D *camPos, struct OCVertex3D *camRot);
struct OCVertex3D NDCSpace(struct OCVertex3D *v, struct OCVertex3D *vNL, struct OCVertex3D *vNR, struct OCVertex3D *vNT, struct OCVertex3D *vNB);
struct OCPoint2D screenSpace(struct OCVertex3D *V, int screenWidth, int screenHeight);
void perspectiveDivision(struct OCVertex3D *v, float frustumNearDist);
struct OCVertex3D rotateAxisX(struct OCVertex3D *V, float rads);
struct OCVertex3D rotateAxisY(struct OCVertex3D *V, float rads);
struct OCVertex3D rotateAxisZ(struct OCVertex3D *V, float rads);
float distanceBetweenVertices(struct OCVertex3D *v1, struct OCVertex3D *v2);
OCVertex3D add(OCVertex3D a, OCVertex3D b);
OCVertex3D subtract(OCVertex3D a, OCVertex3D b);
OCVertex3D scale(OCVertex3D v, float scalar);
float lengthVertex(OCVertex3D v);

OCVertex3D calculateSeparation(int id, OCBoid* boid, OCBoid* boids, int numberBoids, OCSwarmContext* context);
OCVertex3D calculateAlignment(int id, OCBoid* boid, OCBoid* boids, int numberBoids, OCSwarmContext* context);
OCVertex3D calculateCohesion(int id, OCBoid* boid, OCBoid* boids, int numberBoids, OCSwarmContext* context);
OCVertex3D camera_to_ndc(OCVertex3D point, float fov, float aspect_ratio, float near, float far);

__kernel void onUpdate(
    int screenWidth,
    int screenHeight,
    float deltaTime,
    __global unsigned int *video,
    __global OCSwarmContext *context,
    __global OCBoid *boids,
    int numberBoids
)
{
    int id = get_global_id(0);
    if (id < numberBoids) {
        OCBoid *boid = &boids[id];

        OCVertex3D positionCameraSpace = cameraSpace(&boid->position, &context->cameraData.position, &context->cameraData.rotation);
        OCVertex3D positionNDC = camera_to_ndc(positionCameraSpace, 90.0f, 1.77f, 1.0f, 90000.0f);
        OCPoint2D positionScreen = screenSpace(&positionCameraSpace, screenWidth, screenHeight);

        if (positionScreen.x >= 0 && positionScreen.x < screenWidth && positionScreen.y >= 0 && positionScreen.y < screenHeight) {
            int index = positionScreen.y * screenWidth + positionScreen.x;
            video[index] = createRGB(255, 0, 0);
        }
    }
}

unsigned int createRGB(int r, int g, int b)
{
    return (b << 16) + (g << 8) + (r);
}

unsigned int lcg(unsigned int prev) {
    return (1103515245*prev + 12345) % 0x7fffffff;
}

float getRandomFloat(float a, float b, unsigned int *randVal) {
    *randVal = lcg(*randVal);
    return a + (*randVal / (float)0x7fffffff) * (b - a);
}

unsigned int alphaBlend(unsigned int color1, unsigned int color2, unsigned int alpha) {
    unsigned int rb = color1 & 0xff00ff;
    unsigned int g = color1 & 0x00ff00;
    rb += ((color2 & 0xff00ff) - rb) * alpha >> 8;
    g += ((color2 & 0x00ff00) - g) * alpha >> 8;

    return (rb & 0xff00ff) | (g & 0xff00);
}


struct OCVertex3D objectSpace(struct OCVertex3D *A, struct OCVertex3D *objectPosition, struct OCVertex3D *objectRotation, float oScale)
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

struct OCVertex3D cameraSpace(struct OCVertex3D *V, struct OCVertex3D *camPos, struct OCVertex3D *camRot)
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

struct OCVertex3D NDCSpace(struct OCVertex3D *v, struct OCVertex3D *vNL, struct OCVertex3D *vNR, struct OCVertex3D *vNT, struct OCVertex3D *vNB)
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

struct OCPoint2D screenSpace(struct OCVertex3D *V, int screenWidth, int screenHeight)
{
    OCPoint2D A;
    A.x = 0;
    A.y = 0;

    float x = ((1 + V->x) * ((float) screenWidth/2));
    float y = ((1 + V->y) * ((float) screenHeight/2));

    A.x = x;
    A.y = y;

    return A;
}

void perspectiveDivision(struct OCVertex3D *V, float frustumNearDist)
{
    if (V->z != 0) {
        V->x = ( frustumNearDist * V->x) / V->z;
        V->y = ( frustumNearDist * V->y) / V->z;
    }
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

OCVertex3D calculateSeparation(int id, OCBoid* boid, OCBoid* boids, int numberBoids, OCSwarmContext* context) {
    OCVertex3D sum = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
    int count = 0;
    for(int i = 0; i < numberBoids; i++) {
        if (i != id) {
            OCVertex3D difference = subtract(boid->position, boids[i].position);
            float distance = lengthVertex(difference);

            if (distance < context->distanceNeighbor) {
                difference = scale(difference, 1-distance);
                sum = add(sum, difference);
                count++;
            }
        }
    }

    if (count > 0) {
        sum = scale(sum, 1.0f/(float)count);
    }

    return sum;
}

OCVertex3D calculateAlignment(int id, OCBoid* boid, OCBoid* boids, int numberBoids, OCSwarmContext* context) {
    OCVertex3D sum = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
    int count = 0;

    for(int i = 0; i < numberBoids; i++) {
        if(i != id) {
            float distance = lengthVertex(subtract(boid->position, boids[i].position));

            if(distance < context->distanceNeighbor) {
                sum = add(sum, boids[i].velocity);
                count++;
            }
        }
    }

    if (count > 0) {
        sum = scale(sum, 1.0f/(float)count);
    }

    return sum;
}

OCVertex3D calculateCohesion(int id, OCBoid* boid, OCBoid* boids, int numberBoids, OCSwarmContext* context) {
    OCVertex3D sum = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
    int count = 0;

    for(int i = 0; i < numberBoids; i++) {
        if (i != id) {
            OCVertex3D difference = subtract(boids[i].position, boid->position);
            float distance = lengthVertex(difference);

            if(distance < context->distanceNeighbor) {
                sum = add(sum, difference);
                count++;
            }
        }
    }

    if(count > 0) {
        sum = scale(sum, 1.0f/(float)count);
    }

    return sum;
}

float distanceBetweenVertices(struct OCVertex3D *v1, struct OCVertex3D *v2)
{
    return sqrt( (v2->x - v1->x) * (v2->x - v1->x) + (v2->y - v1->y) * (v2->y - v1->y) + (v2->z - v1->z) * (v2->z - v1->z));
}

OCVertex3D add(OCVertex3D a, OCVertex3D b) {
    OCVertex3D result;
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    result.z = a.z + b.z;
    result.u = a.u + b.u;
    result.v = a.v + b.v;
    return result;
}

OCVertex3D subtract(OCVertex3D a, OCVertex3D b) {
    OCVertex3D result;
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    result.z = a.z - b.z;
    result.u = a.u - b.u;
    result.v = a.v - b.v;
    return result;
}

OCVertex3D scale(OCVertex3D v, float scalar) {
    OCVertex3D result;
    result.x = v.x * scalar;
    result.y = v.y * scalar;
    result.z = v.z * scalar;
    result.u = v.u * scalar;
    result.v = v.v * scalar;
    return result;
}

float lengthVertex(OCVertex3D v) {
    return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

OCVertex3D camera_to_ndc(OCVertex3D point, float fov, float aspect_ratio, float near, float far) {
    float f = 1.0f / tan(fov / 2.0f);
    OCVertex3D ndc;

    // Perspective division: divide by -point.z (change of sign because camera looks towards -z)
    ndc.x = (f / aspect_ratio) * point.x / -point.z;
    ndc.y = f * point.y / -point.z;

    // Convert z from [near, far] to [-1, 1]
    ndc.z = (near + far) / (near - far) + (1 / point.z) * (2 * far * near) / (near - far);

    return ndc;
}