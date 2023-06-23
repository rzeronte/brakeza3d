typedef struct OCVertex3D {
    float x;
    float y;
    float z;
    float u;
    float v;
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

unsigned int createRGB(int r, int g, int b);
float distanceBetweenVertices(struct OCVertex3D *v1, struct OCVertex3D *v2);
float getRandomFloat(float a, float b, unsigned int *randVal);
unsigned int lcg(unsigned int prev);
void increase(struct OCVertex3D *a, OCVertex3D b);
void decrease(struct OCVertex3D *a, OCVertex3D b);
void scale(struct OCVertex3D *a, OCVertex3D b);
unsigned int alphaBlend(unsigned int color1, unsigned int color2, unsigned int alpha);

__constant float timeScale = 0.01f; // Ajusta la velocidad de la variación de offset

__kernel void onUpdate(
    int screenWidth,
    int screenHeight,
    float time,
    __global unsigned int *video,
    __global bool *stencil,
    __global unsigned int *bufferDepth,
    __global unsigned int *texture,
    int offsetX,
    int offsetY
)
{
    int i = get_global_id(0);

    int x = i % screenWidth;
    int y = i / screenWidth;

    // Calcular offsets basados en el tiempo
    int offsetXRed = offsetX;
    int offsetYRed = offsetY;

    int offsetXGreen = -offsetXRed; // Oscila en la dirección opuesta a offsetXRed
    int offsetYGreen = -offsetYRed; // Oscila en la dirección opuesta a offsetYRed

    // Asegúrate de que el desplazamiento no nos lleve fuera de los límites de la pantalla.
    if ((x + offsetXRed < 0 || x + offsetXRed >= screenWidth || y + offsetYRed < 0 || y + offsetYRed >= screenHeight))
        return;

    // Desplazamos la posición para la capa roja y la capa verde.
    int offsetIndexRed = (y + offsetYRed) * screenWidth + (x + offsetXRed);

    float2 uv = { x, y };
    float2 resolution = { screenWidth, screenHeight };
    float2 st = uv / resolution;

    // Si el valor del stencil en esta posición es verdadero, aplicamos el efecto.
    if (stencil[i]) {
        float depth = bufferDepth[i] / 10000000;

        float2 nst = st;
        float2 offset = {0,1};
        offset *= time * 0.05f;
        nst += offset;

        float intPart;
        nst.x = fract(nst.x, &intPart);
        nst.y = fract(nst.y, &intPart);

        int cx = nst.x * screenWidth;
        int cy = nst.y * screenHeight;
        int indexT = cy * screenWidth + cx;

        float alpha = 0.5f + 0.1f * sin(time);
        video[i] = alphaBlend(video[i], texture[indexT], 144 * alpha);

        // Definir el tamaño de la aura
        int auraSize = 8;

        // Loop para crear un borde difuso alrededor del objeto
        for(int auraX = x - auraSize; auraX <= x + auraSize; auraX++) {
            for (int auraY = y - auraSize; auraY <= y + auraSize; auraY++) {

                // Verificar que no estamos fuera del área de la pantalla
                if (auraX < 0 || auraY < 0 || auraX >= screenWidth || auraY >= screenHeight) continue;

                // Calcular el índice del pixel
                int auraIndex = auraY * screenWidth + auraX;

                // Verificar que el pixel no está dentro del objeto
                if (stencil[auraIndex]) continue;

                // Calcular la distancia a la posición original para la intensidad
                int dx = auraX - x;
                int dy = auraY - y;
                float distance = sqrt((float) (dx * dx + dy * dy));
                float intensity = 1.0 - (distance / auraSize);

                // Suavizar la intensidad con la función smoothstep
                float smoothedIntensity = smoothstep(0.005f, 0.5f, intensity);

                // Usar la intensidad suavizada al crear el color del aura
                int auraR = 0;
                int auraG = 255 * smoothedIntensity;
                int auraB = 0;

                // Crear el color del aura y aplicar alpha blending
                unsigned int auraColor = createRGB(auraR, auraG, auraB);
                video[auraIndex] = alphaBlend(video[auraIndex], auraColor, 144 * smoothedIntensity);
            }
        }
    }
}

unsigned int createRGB(int r, int g, int b)
{
    return (b << 16) + (g << 8) + (r);
}

float distanceBetweenVertices(struct OCVertex3D *v1, struct OCVertex3D *v2)
{
    return sqrt( (v2->x - v1->x) * (v2->x - v1->x) + (v2->y - v1->y) * (v2->y - v1->y) + (v2->z - v1->z) * (v2->z - v1->z));
}

float getRandomFloat(float a, float b, unsigned int *randVal) {
    *randVal = lcg(*randVal);
    return a + (*randVal / (float)0x7fffffff) * (b - a);
}

unsigned int lcg(unsigned int prev) {
    return (1103515245*prev + 12345) % 0x7fffffff;
}

void increase(struct OCVertex3D *a, OCVertex3D b)
{
    a->x += b.x;
    a->y += b.y;
    a->z += b.z;
}

void decrease(struct OCVertex3D *a, OCVertex3D b)
{
    a->x -= b.x;
    a->y -= b.y;
    a->z -= b.z;
}

void scale(struct OCVertex3D *a, OCVertex3D b)
{
    a->x *= b.x;
    a->y *= b.y;
    a->z *= b.z;
}

unsigned int alphaBlend(unsigned int color1, unsigned int color2, unsigned int alpha)
{
    unsigned int rb = color1 & 0xff00ff;
    unsigned int g = color1 & 0x00ff00;
    rb += ((color2 & 0xff00ff) - rb) * alpha >> 8;
    g += ((color2 & 0x00ff00) - g) * alpha >> 8;

    return (rb & 0xff00ff) | (g & 0xff00);
}