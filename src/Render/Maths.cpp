
#include "../../headers/Render/Maths.h"
#include "../../headers/Render/Drawable.h"
#include "../../headers/Render/M3.h"
#include "../../headers/Render/Transforms.h"
#include "../../headers/Render/EngineBuffers.h"
#include "../../headers/Render/Tools.h"


float Maths::degreesToRadians(float angleDegrees)
{
    return angleDegrees * (float) M_PI / (float) 180.0;;
}

float Maths::radiansToDegrees(float angleRadians)
{
    return angleRadians * (float) 180.0 / (float) M_PI;;
}

Vertex3D Maths::rotateVertex(Vertex3D V, M3 rotation)
{
    Vertex3D B = rotation * V;
    B.u = V.u; B.v = V.v;

    return B;
}

// https://elcodigografico.wordpress.com/2014/03/29/coordenadas-baricentricas-en-triangulos/
float Maths::barycentricSide(int x, int y, Point2D pa, Point2D pb)
{
    return (pa.y - pb.y) * x + (pb.x - pa.x) * y + pa.x*pb.y - pb.x*pa.y;
}

void Maths::getBarycentricCoordinates(float &alpha, float &theta, float &gamma, int x, int y, Point2D v1, Point2D v2,
                                       Point2D v3)
{
    alpha = Maths::barycentricSide( x, y, v2, v3 ) / Maths::barycentricSide( v1.x, v1.y, v2, v3 );
    theta = Maths::barycentricSide( x, y, v3, v1 ) / Maths::barycentricSide( v2.x, v2.y, v3, v1 );
    gamma = Maths::barycentricSide( x, y, v1, v2 ) / Maths::barycentricSide( v3.x, v3.y, v1, v2 );

}

bool Maths::isVector3DClippingPlane(Plane P, Vector3D V)
{
    // El clipping solo se realiza en Line2Ds que están parcialmente fuera
    // es decir, ningun punto dentro o  no están los 2 dentro
    float min_distance_to_clipping = EngineSetup::getInstance()->FRUSTUM_CLIPPING_DISTANCE;

    if (! (P.distance(V.vertex1) > min_distance_to_clipping && P.distance(V.vertex2) > min_distance_to_clipping)
        &&
        ! (P.distance(V.vertex1) < min_distance_to_clipping && P.distance(V.vertex2) < min_distance_to_clipping )
            ) {
        return true;
    }

    return false;
}

Vertex3D Maths::getCenterVertices(Vertex3D vertices[], int num_vertices) {
    Vertex3D middle = Vertex3D(0, 0, 0);

    for (int i = 0; i < num_vertices; i++) {
        middle.x += vertices[i].x;
        middle.y += vertices[i].y;
        middle.z += vertices[i].z;
    }

    middle.x/= num_vertices;
    middle.y/= num_vertices;
    middle.z/= num_vertices;

    return middle;
}

void Maths::sortVertexByY(Vertex3D &A, Vertex3D &B, Vertex3D &C)
{
    int n = 3;
    Vertex3D v[3];
    v[0] = A; v[1] = B; v[2] = C;

    for (int i = 1 ; i < n; i++) {
        for (int j = 0 ; j < (n - i); j++) {
            if (v[j].y > v[j+1].y) {
                Vertex3D aux = v[j];
                v[j] = v[j+1];
                v[j+1] = aux;
            }
        }
    }

    A = v[0]; B = v[1]; C = v[2];

    return;
}

void Maths::sortVertexByX(Vertex3D &A, Vertex3D &B, Vertex3D &C)
{
    int n = 3;
    Vertex3D v[3];
    v[0] = A; v[1] = B; v[2] = C;

    for (int i = 1 ; i < n; i++) {
        for (int j = 0 ; j < (n - i); j++) {
            if (v[j].x > v[j+1].x) {
                Vertex3D aux = v[j];
                v[j] = v[j+1];
                v[j+1] = aux;
            }
        }
    }

    A = v[0]; B = v[1]; C = v[2];

    return;
}

void Maths::sortPointsByY(Point2D &A, Point2D &B, Point2D &C)
{
    int n = 3;
    Point2D p[3];
    p[0] = A; p[1] = B; p[2] = C;


    for (int i = 1 ; i < n ; i++) {
        for (int j = 0 ; j < (n - i) ; j++) {
            if (p[j].y > p[j+1].y) {
                Point2D aux = p[j];
                p[j] = p[j+1];
                p[j+1] = aux;
            }
        }
    }

    A = p[0]; B = p[1]; C = p[2];

    return;
}

void Maths::sortPointsByX(Point2D &A, Point2D &B, Point2D &C)
{
    int n = 3;
    Point2D p[3];
    p[0] = A; p[1] = B; p[2] = C;

    for (int i = 1 ; i < n ; i++) {
        for (int j = 0 ; j < (n - i) ; j++) {
            if (p[j].x > p[j+1].x) {
                Point2D aux = p[j];
                p[j] = p[j+1];
                p[j+1] = aux;
            }
        }
    }

    A = p[0]; B = p[1]; C = p[2];
}

void Maths::VertexSwap(Vertex3D vertexes[], int i, int j)
{
    Vertex3D tmp = vertexes[i];
    vertexes[i] = vertexes[j];
    vertexes[j] = tmp;
}

void Maths::sortVertexesByX(Vertex3D vertexes[], int N)
{
    int i, j, k;
    for (i = 0; i < N - 1; i++)
    {
        for (k = i, j = i + 1; j < N; j++) {
            if (vertexes[j].x < vertexes[k].x) {
                k = j;
            }
        }
        Maths::VertexSwap (vertexes, i, k);
    }
}

void Maths::sortVertexesByAngles(Vertex3D vertexes[], int N)
{
    int i, j, k;

    for (i = 0; i < N - 1; i++) {
        for (k = i, j = i + 1; j < N; j++) {
            if (vertexes[j].angle < vertexes[k].angle) {
                k = j;
            }
        }
        Maths::VertexSwap (vertexes, i, k);
    }
}

void Maths::sortVertexesByY(Vertex3D vertexes[], int N)
{
    int i, j, k;
    for (i = 0; i < N - 1; i++)
    {
        for (k = i, j = i + 1; j < N; j++) {
            if (vertexes[j].x < vertexes[k].x) {
                k = j;
            }
        }
        Maths::VertexSwap (vertexes, i, k);
    }
}

float Maths::distanteBetweenpoints(Vertex3D v1, Vertex3D v2)
{

    float abs_vector = sqrtf( (v2.x - v1.x)*(v2.x - v1.x) + (v2.y - v1.y)*(v2.y - v1.y) + (v2.z - v1.z)*(v2.z - v1.z) );

    return abs_vector;
}

Uint32 Maths::mixColor(Uint32 color, float distance, LightPoint3D *lp, Vertex3D Q)
{

    Vertex3D P = *lp->getPosition();
    Vertex3D R = lp->AxisForward();

    Vector3D L = Vector3D(P, Q);
    Vertex3D Lv = L.getUnitVector();

    const float min = Vertex3D::dotProduct(R, Lv);

    float p = 100;
    float max = fmaxf(min, 0);
    float pow = powf(max, p);

    float intensity = pow / (lp->kc + lp->kl*distance + lp->kq * (distance * distance));

    int r_light = (int) (Tools::getRedValueFromColor(lp->color)   * intensity);
    int g_light = (int) (Tools::getGreenValueFromColor(lp->color) * intensity);
    int b_light = (int) (Tools::getBlueValueFromColor(lp->color)  * intensity);

    int r_original = (int) (Tools::getRedValueFromColor(color) * ( 1 - intensity) );
    int g_original = (int) (Tools::getGreenValueFromColor(color) * ( 1 - intensity) );
    int b_original = (int) (Tools::getBlueValueFromColor(color) * ( 1 - intensity) );

    Uint32 c = Tools::createRGB(
            r_light + r_original,
            g_light + g_original,
            b_light + b_original
    );

    return c;
}

Vertex3D Maths::crossProduct(Vertex3D u, Vertex3D v)
{
    Vertex3D V;

    V.x = (u.y * v.z) - (u.z * v.y);
    V.y = (u.z * v.x) - (u.x * v.z);
    V.z = (u.x * v.y) - (u.y * v.x);

    return V;
}

float Maths::getHorizontalAngleBetweenObject3DAndCamera(Object3D *o1, Camera3D *cam)
{
    Vertex3D oRight = o1->AxisForward();
    Vertex3D R = cam->AxisForward();

    float rads = acosf(  Vertex3D::dotProduct(R, oRight) / (R.getModule() * oRight.getModule()) );

    float degs = Maths::radiansToDegrees(rads);

    float dot2d = oRight.x * R.y - oRight.y * R.x;

    if (signbit(dot2d)) {
        degs =  360 - degs;
    }

    return degs;
}

