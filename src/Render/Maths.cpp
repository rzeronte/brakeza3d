
#include "../../headers/Render/Maths.h"
#include "../../headers/Render/Drawable.h"
#include "../../headers/Render/Transforms.h"

float Maths::degreesToRadians(float angleDegrees) {
    return angleDegrees * (float) M_PI / (float) 180.0;
}

float Maths::radiansToDegrees(float angleRadians) {
    return angleRadians * (float) 180.0 / (float) M_PI;
}

// https://elcodigografico.wordpress.com/2014/03/29/coordenadas-baricentricas-en-triangulos/
float Maths::barycentricSide(int x, int y, Point2D pa, Point2D pb) {
    return (pa.y - pb.y) * x + (pb.x - pa.x) * y + pa.x * pb.y - pb.x * pa.y;
}

void Maths::getBarycentricCoordinates(float &alpha, float &theta, float &gamma, int x, int y, Point2D v1, Point2D v2,
                                      Point2D v3) {
    alpha = Maths::barycentricSide(x, y, v2, v3) / Maths::barycentricSide(v1.x, v1.y, v2, v3);
    theta = Maths::barycentricSide(x, y, v3, v1) / Maths::barycentricSide(v2.x, v2.y, v3, v1);
    gamma = Maths::barycentricSide(x, y, v1, v2) / Maths::barycentricSide(v3.x, v3.y, v1, v2);
}

void
Maths::getBarycentricCoordinatesPrecalc(float &alpha, float &theta, float &gamma, int x, int y, Point2D v1, Point2D v2,
                                        Point2D v3, float r1, float r2, float r3) {
    alpha = Maths::barycentricSide(x, y, v2, v3) / r1;
    theta = Maths::barycentricSide(x, y, v3, v1) / r2;
    gamma = Maths::barycentricSide(x, y, v1, v2) / r3;
}

/**
 * 0 = dos vértices dentro
 * 1 = ningún vértice dentro
 * 2 = vértice A dentro
 * 3 = vértice B dentro
 */
int Maths::isVector3DClippingPlane(Plane &P, Vector3D &V) {
    EngineSetup *SETUP = EngineSetup::getInstance();

    float distanceV1 = P.distance(V.vertex1);
    float distanceV2 = P.distance(V.vertex2);

    if (distanceV1 > SETUP->FRUSTUM_CLIPPING_DISTANCE && distanceV2 > SETUP->FRUSTUM_CLIPPING_DISTANCE) {
        return 1;
    }

    if (distanceV2 > SETUP->FRUSTUM_CLIPPING_DISTANCE && distanceV1 < SETUP->FRUSTUM_CLIPPING_DISTANCE) {
        return 2;
    }

    if (distanceV1 > SETUP->FRUSTUM_CLIPPING_DISTANCE && distanceV2 < SETUP->FRUSTUM_CLIPPING_DISTANCE) {
        return 3;
    }

    return 0;
}

Vertex3D Maths::getCenterVertices(Vertex3D vertices[], int num_vertices) {
    Vertex3D middle = Vertex3D(0, 0, 0);

    for (int i = 0; i < num_vertices; i++) {
        middle.x += vertices[i].x;
        middle.y += vertices[i].y;
        middle.z += vertices[i].z;
        middle.u += vertices[i].u;
        middle.v += vertices[i].v;
    }

    middle.x /= num_vertices;
    middle.y /= num_vertices;
    middle.z /= num_vertices;
    middle.u /= num_vertices;
    middle.v /= num_vertices;

    return middle;
}

int Maths::orient2d(const Point2D &a, const Point2D &b, const Point2D &c) {
    return (c.x - a.x) * (b.y - a.y) - (c.y - a.y) * (b.x - a.x);
}

void Maths::sortVerticesByY(Vertex3D &A, Vertex3D &B, Vertex3D &C) {
    const int n = 3;
    Vertex3D v[3];
    v[0] = A;
    v[1] = B;
    v[2] = C;

    for (int i = 1; i < n; i++) {
        for (int j = 0; j < (n - i); j++) {
            if (v[j].y > v[j + 1].y) {
                Vertex3D aux = v[j];
                v[j] = v[j + 1];
                v[j + 1] = aux;
            }
        }
    }

    A = v[0];
    B = v[1];
    C = v[2];
}

void Maths::sortVerticesByX(Vertex3D &A, Vertex3D &B, Vertex3D &C) {
    const int n = 3;
    Vertex3D v[3];
    v[0] = A;
    v[1] = B;
    v[2] = C;

    for (int i = 1; i < n; i++) {
        for (int j = 0; j < (n - i); j++) {
            if (v[j].x > v[j + 1].x) {
                Vertex3D aux = v[j];
                v[j] = v[j + 1];
                v[j + 1] = aux;
            }
        }
    }

    A = v[0];
    B = v[1];
    C = v[2];
}

void Maths::sortPointsByY(Point2D &A, Point2D &B, Point2D &C) {
    const int n = 3;
    Point2D p[3];
    p[0] = A;
    p[1] = B;
    p[2] = C;

    for (int i = 1; i < n; i++) {
        for (int j = 0; j < (n - i); j++) {
            if (p[j].y > p[j + 1].y) {
                Point2D aux = p[j];
                p[j] = p[j + 1];
                p[j + 1] = aux;
            }
        }
    }

    A = p[0];
    B = p[1];
    C = p[2];
}

void Maths::sortPointsByX(Point2D &A, Point2D &B, Point2D &C) {
    const int n = 3;
    Point2D p[3];
    p[0] = A;
    p[1] = B;
    p[2] = C;

    for (int i = 1; i < n; i++) {
        for (int j = 0; j < (n - i); j++) {
            if (p[j].x > p[j + 1].x) {
                Point2D aux = p[j];
                p[j] = p[j + 1];
                p[j + 1] = aux;
            }
        }
    }

    A = p[0];
    B = p[1];
    C = p[2];
}

void Maths::VertexSwap(Vertex3D vertexes[], int i, int j) {
    Vertex3D tmp = vertexes[i];
    vertexes[i] = vertexes[j];
    vertexes[j] = tmp;
}

void Maths::sortVerticesByX(Vertex3D *vertexes, int N) {
    int i, j, k;
    for (i = 0; i < N - 1; i++) {
        for (k = i, j = i + 1; j < N; j++) {
            if (vertexes[j].x < vertexes[k].x) {
                k = j;
            }
        }
        Maths::VertexSwap(vertexes, i, k);
    }
}

void Maths::sortVerticesByY(Vertex3D *vertexes, int N) {
    int i, j, k;
    for (i = 0; i < N - 1; i++) {
        for (k = i, j = i + 1; j < N; j++) {
            if (vertexes[j].x < vertexes[k].x) {
                k = j;
            }
        }
        Maths::VertexSwap(vertexes, i, k);
    }
}

float Maths::distanceBetweenVertices(Vertex3D v1, Vertex3D v2) {
    return sqrtf((v2.x - v1.x) * (v2.x - v1.x) + (v2.y - v1.y) * (v2.y - v1.y) + (v2.z - v1.z) * (v2.z - v1.z));
}

float Maths::getHorizontalAngleBetweenObject3DAndCamera(Object3D *object, Camera3D *cam) {
    Vertex3D a = cam->getPosition() - object->getPosition();
    Vertex3D b = object->getRotation() * EngineSetup::getInstance()->forward;

    a = a.getNormalize();

    float theta = acos(a * b);
    theta = Maths::radiansToDegrees(theta);

    if (a.x * a.z < 0) {
        theta = 360.0f - theta;
    }

    return theta;
}

long Maths::GetNextActive(long x, long vertexCount, const bool *active) {
    for (;;) {
        if (++x == vertexCount) x = 0;
        if (active[x])
            return (x);
    }
}

long Maths::GetPrevActive(long x, long vertexCount, const bool *active) {
    for (;;) {
        if (--x == -1) x = vertexCount - 1;
        if (active[x])
            return (x);
    }
}

int Maths::TriangulatePolygon(long vertexCount, Vertex3D *vertices, Vertex3D normal, std::vector<Triangle *> &triangles,
                              Object3D *parent, Texture *texture, Texture *lightmap, bool clipped, bool isBsp,
                              const unsigned char lightstyle[4]) {
    bool *active = new bool[vertexCount];
    for (long a = 0; a < vertexCount; a++) active[a] = true;
    int triangleCount = 0;
    int start = 0;
    int p1 = 0;
    int p2 = 1;
    int m1 = vertexCount - 1;
    int m2 = vertexCount - 2;
    bool lastPositive = false;
    for (;;) {
        if (p2 == m2) {
            // Only three vertices remain.
            // Triangle creation
            Vertex3D tv1 = Transforms::objectToLocal(vertices[m1], parent);
            Vertex3D tv2 = Transforms::objectToLocal(vertices[p1], parent);
            Vertex3D tv3 = Transforms::objectToLocal(vertices[p2], parent);

            auto *t = new Triangle(tv1, tv2, tv3, parent);
            t->setTexture(texture);
            t->setLightmap(lightmap);
            t->setClipped(clipped);
            t->setId(triangles.size());
            t->isBSP = isBsp;
            t->typelight[0] = lightstyle[0];
            t->typelight[1] = lightstyle[1];
            t->typelight[2] = lightstyle[2];
            t->typelight[3] = lightstyle[3];

            triangles.emplace_back(t);

            triangleCount++;
            break;
        }

        Vertex3D vp1 = vertices[p1];
        Vertex3D vp2 = vertices[p2];
        Vertex3D vm1 = vertices[m1];
        Vertex3D vm2 = vertices[m2];

        bool positive = false;
        bool negative = false;

        // Determine whether vp1, vp2, and vm1 form a valid triangles.
        Vertex3D n1 = normal % (vm1 - vp2).getNormalize();
        if (n1 * (vp1 - vp2) > EngineSetup::getInstance()->EPSILON) {
            positive = true;
            Vertex3D n2 = (normal % (vp1 - vm1).getNormalize());
            Vertex3D n3 = (normal % (vp2 - vp1).getNormalize());
            for (long a = 0; a < vertexCount; a++) {
                // Look for other vertices inside the triangles.
                if ((active[a]) && (a != p1) && (a != p2) && (a != m1)) {
                    Vertex3D v = vertices[a];
                    if ((n1 * (v - vp2).getNormalize() > -EngineSetup::getInstance()->EPSILON)
                        && (n2 * (v - vm1).getNormalize() > -EngineSetup::getInstance()->EPSILON)
                        && (n3 * (v - vp1).getNormalize() > -EngineSetup::getInstance()->EPSILON)) {
                        positive = false;
                        break;
                    }
                }
            }
        }

        // Determine whether vm1, vm2, and vp1 form a valid triangles.
        n1 = normal % (vm2 - vp1).getNormalize();
        if (n1 * (vm1 - vp1) > EngineSetup::getInstance()->EPSILON) {
            negative = true;
            Vertex3D n2 = (normal % (vm1 - vm2).getNormalize());
            Vertex3D n3 = (normal % (vp1 - vm1).getNormalize());
            for (long a = 0; a < vertexCount; a++) {
                // Look for other vertices inside the triangles.
                if ((active[a]) && (a != m1) && (a != m2) && (a != p1)) {
                    Vertex3D v = vertices[a];
                    if ((n1 * (v - vp1).getNormalize() > -EngineSetup::getInstance()->EPSILON)
                        && (n2 * (v - vm2).getNormalize() > -EngineSetup::getInstance()->EPSILON)
                        && (n3 * (v - vm1).getNormalize() > -EngineSetup::getInstance()->EPSILON)) {
                        negative = false;
                        break;
                    }
                }
            }
        }

        // If both triangles are valid, choose the one having
        // the larger smallest angle.
        if ((positive) && (negative)) {
            float pd = (vp2 - vm1).getNormalize() * (vm2 - vm1).getNormalize();
            float md = (vm2 - vp1).getNormalize() * (vp2 - vp1).getNormalize();
            if (fabs(pd - md) < EngineSetup::getInstance()->EPSILON) {
                if (lastPositive) positive = false;
                else negative = false;
            } else {
                if (pd < md) negative = false;
                else positive = false;
            }
        }

        if (positive) {
            // Output the triangles m1, p1, p2.
            active[p1] = false;

            Vertex3D tv1 = Transforms::objectToLocal(vertices[m1], parent);
            Vertex3D tv2 = Transforms::objectToLocal(vertices[p1], parent);
            Vertex3D tv3 = Transforms::objectToLocal(vertices[p2], parent);

            auto *t = new Triangle(tv1, tv2, tv3, parent);
            t->setLightmap(lightmap);
            t->setTexture(texture);
            t->setClipped(clipped);
            t->setId(triangles.size());
            t->isBSP = isBsp;
            t->typelight[0] = lightstyle[0];
            t->typelight[1] = lightstyle[1];
            t->typelight[2] = lightstyle[2];
            t->typelight[3] = lightstyle[3];

            triangles.emplace_back(t);

            triangleCount++;
            //triangles++;
            p1 = GetNextActive(p1, vertexCount, active);
            p2 = GetNextActive(p2, vertexCount, active);
            lastPositive = true;
            start = -1;
        } else if (negative) {
            // Output the triangles m2, m1, p1.
            active[m1] = false;

            Vertex3D tv1 = Transforms::objectToLocal(vertices[m2], parent);
            Vertex3D tv2 = Transforms::objectToLocal(vertices[m1], parent);
            Vertex3D tv3 = Transforms::objectToLocal(vertices[p1], parent);

            auto *t = new Triangle(tv1, tv2, tv3, parent);
            t->setLightmap(lightmap);
            t->setTexture(texture);
            t->setClipped(clipped);
            t->setId(triangles.size());
            t->isBSP = isBsp;
            t->typelight[0] = lightstyle[0];
            t->typelight[1] = lightstyle[1];
            t->typelight[2] = lightstyle[2];
            t->typelight[3] = lightstyle[3];

            triangles.emplace_back(t);

            triangleCount++;
            //triangles++;
            m1 = GetPrevActive(m1, vertexCount, active);
            m2 = GetPrevActive(m2, vertexCount, active);
            lastPositive = false;
            start = -1;
        } else {
            // Exit if we've gone all the way around the
            // polygon without finding a valid triangles.
            if (start == -1) start = p2;
            else if (p2 == start) {
                break;
            }

            // Advance working set of vertices.
            m2 = m1;
            m1 = p1;
            p1 = p2;
            p2 = GetNextActive(p2, vertexCount, active);
        }
    }

    delete[] active;
    return (triangleCount);
}

bool
Maths::ClippingPolygon(Vertex3D *input, int numInput, Vertex3D *output, int &numOutput, int id_plane, Plane *planes) {
    Vector3D edge;

    bool new_vertices = false;

    for (int i = 0; i < numInput; i++) {
        if (i + 1 < numInput) {
            edge = Vector3D(input[i], input[i + 1]);
        } else {
            edge = Vector3D(input[i], input[0]);
        }
        // test clip plane
        const int testClip = Maths::isVector3DClippingPlane(planes[id_plane], edge);

        /** 0 = dos vértices dentro | 1 = ningún vértice dentro | 2 = vértice A dentro | 3 = vértice B dentro */
        // Si el primer vértice está dentro, lo añadimos a la salida
        if (testClip == 0 || testClip == 2) {
            output[numOutput] = edge.vertex1;
            numOutput++;
        }

        // Si el primer y el segundo vértice no tienen el mismo estado, añadimos el punto de intersección al plano
        if (testClip > 1) {
            float t = 0;    // [0,1] range point intersección
            output[numOutput] = planes[id_plane].getPointIntersection(edge.vertex1, edge.vertex2, t);
            output[numOutput].u = edge.vertex1.u + t * (edge.vertex2.u - edge.vertex1.u);
            output[numOutput].v = edge.vertex1.v + t * (edge.vertex2.v - edge.vertex1.v);

            numOutput++;
            new_vertices = true;
        }
    }

    return new_vertices;
}

float Maths::distancePointVector(Vertex3D Q, Vector3D L) {
    Vertex3D d = L.getComponent().getNormalize();

    Vector3D firstL_Q = Vector3D(L.vertex1, Q);
    Vertex3D v = firstL_Q.getComponent();

    float t = v * d;

    Vertex3D P;

    P.x = L.vertex1.x + (t * d.x);
    P.y = L.vertex1.y + (t * d.y);
    P.z = L.vertex1.z + (t * d.z);

    Vertex3D AP = P - Q;

    float distance = AP.getModule();

    // Camera-triangle vector
    Vertex3D z = L.vertex1 - Q;

    if ((z * (z % AP)) >= 0) {
        return distance;
    }

    return -distance;
}

bool Maths::sameSide(Vertex3D p1, Vertex3D p2, Vertex3D a, Vertex3D b) {
    Vertex3D cp1 = (b - a) % (p1 - a);
    Vertex3D cp2 = (b - a) % (p2 - a);

    if ((cp1 * cp2) >= 0) {
        return true;
    }

    return false;
}

bool Maths::PointInTriangle(Vertex3D p, Vertex3D a, Vertex3D b, Vertex3D c) {
    return sameSide(p, a, b, c) && sameSide(p, b, a, c) && sameSide(p, c, a, b);
}

float Maths::TriangleArea(float dX0, float dY0, float dX1, float dY1, float dX2, float dY2) {
    float area = ((dX1 - dX0) * (dY2 - dY0) - (dX2 - dX0) * (dY1 - dY0)) / 2;

    return abs(area);
}

float Maths::normalizeToRange(float value, float min, float max) {
    if ((max - min) == 0) {
        return 0;
    }

    return (value - min) / (max - min);
}

float Maths::sqrt1(const float &n) {
    static union {
        int i;
        float f;
    } u;
    u.i = 0x5F375A86 - (*(int *) &n >> 1);
    return (int(3) - n * u.f * u.f) * n * u.f * 0.5f;
}

