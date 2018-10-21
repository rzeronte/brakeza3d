
#include "../../headers/Maths.h"
#include "../../headers/Drawable.h"
#include "../../headers/M3.h"
#include "../../headers/Transforms.h"
#include "../../headers/EngineBuffers.h"
#include "../../headers/Tools.h"


float Maths::degreesToRadians(float angleDegrees)
{
    float rads;

    rads = angleDegrees * (float) M_PI / (float) 180.0;

    return rads;
}

float Maths::radiansToDegrees(float angleRadians)
{
    float degrees;

    degrees = angleRadians * (float) 180.0 / (float) M_PI;

    return degrees;
}

Vertex Maths::rotateVertex(Vertex V, Vertex rotation)
{

    M3 MRX = M3::RX(rotation.x);
    M3 MRY = M3::RY(rotation.y);
    M3 MRZ = M3::RZ(rotation.z);

    Vertex B = (MRX * MRY * MRZ) * V;

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


bool Maths::triangulate(Vertex vertexes[], int num_vertex, Object3D *parent, Camera *cam, Vertex A, Vertex B, Vertex C, Texture *texture)
{

    // Usamos un vértice arbitrario para trazar un radio
    // hacia el vértice 0
    Vertex middle = Maths::getCenterVertices(vertexes, num_vertex);
    Vector3D arbitrary_vector = Vector3D(middle, vertexes[0]);
    //Drawable::drawVector3D(screen, arbitrary_vector, cam, Color::yellow());

    // Vamos recorriendo cada 'radio', salvo el primero (i > 0), cuyo radio contra si mismo será 0.
    // Estámos organizando los vértices clockwise
    for (int i = 0; i < num_vertex; i++) {
        float angle = 0;
        float dot;

        // Ya utilizo el primer vertice como radio "referencia" por tanto sé q su angulo es 0. Puedo ignorar su cálculo
        if (i > 0) {
            Vector3D ratio = Vector3D(middle, vertexes[i]);
            //Drawable::drawVector3D(screen, ratio, cam, Color::pink());

            Vertex tmp1 = arbitrary_vector.getComponent();
            Vertex tmp2 = ratio.getComponent();

            float numerador = (tmp1.x * tmp2.x) + (tmp1.y * tmp2.y) + (tmp1.z * tmp2.z);
            float denominador = sqrt((tmp1.x * tmp1.x) + (tmp1.y * tmp1.y) + (tmp1.z * tmp1.z)) *
                                sqrt((tmp2.x * tmp2.x) + (tmp2.y * tmp2.y) + (tmp2.z * tmp2.z));
            float cos_angle_vectors = numerador / denominador;
            angle = acos(cos_angle_vectors);

            dot = tmp1.x * tmp2.y - tmp1.y * tmp2.x;

            if (dot < 0) {
                angle = angle * -1;
            }

            angle = Maths::radiansToDegrees(angle);
        }

        vertexes[i].angle = angle;
        //std::cout << "Angle vertex " << i << " " << label << ", deg: " << angle << std::endl;
    }

    Maths::sortVertexesByAngles(vertexes, num_vertex);

    Vertex Ao = A;
    Vertex Bo = B;
    Vertex Co = C;

    Ao = Transforms::objectSpace(Ao, parent);
    Bo = Transforms::objectSpace(Bo, parent);
    Co = Transforms::objectSpace(Co, parent);

    // Pasamos por la cámara
    Ao = Transforms::cameraSpace(Ao, cam);
    Bo = Transforms::cameraSpace(Bo, cam);
    Co = Transforms::cameraSpace(Co, cam);

    Ao = Transforms::homogeneousClipSpace(Ao, cam);
    Bo = Transforms::homogeneousClipSpace(Bo, cam);
    Co = Transforms::homogeneousClipSpace(Co, cam);

    // y obtenemos los puntos en la proyección 2d
    Point2D pa = Transforms::screenSpace(Ao, cam);
    Point2D pb = Transforms::screenSpace(Bo, cam);
    Point2D pc = Transforms::screenSpace(Co, cam);

    float alpha, theta, gamma;

    // triangulamos con un sencillo algoritmo que recorre los vértices: 012, 023, 034...
    int current = 1;
    while(current < num_vertex-1 ) {
        int next = current + 1;

        if (next+1 <= num_vertex) {
            //printf("init: %d, current: %d, next: %d,: num_vertex: %d\r\n", 0, current, next, num_vertex);
            EngineBuffers::getInstance()->trianglesClippingCreated++;

            Drawable::drawVertex(vertexes[0], cam, Color::red());
            Drawable::drawVertex(vertexes[current], cam, Color::green());
            Drawable::drawVertex(vertexes[next], cam, Color::blue());

            // Vertex new triangles
            Vertex tv1 = Transforms::objectToLocal(vertexes[0], parent);
            Vertex tv2 = Transforms::objectToLocal(vertexes[current], parent);
            Vertex tv3 = Transforms::objectToLocal(vertexes[next], parent);

            //Tools::sortVertexByX(tv1, tv2,tv3);
            Maths::sortVertexByY(tv1, tv2,tv3);

            // Vamos a calcular las coordenadas UV para tv1, tv2 y tv3
            Vertex nv1 = tv1;
            Vertex nv2 = tv2;
            Vertex nv3 = tv3;

            // Pasamos por la cámara
            nv1 = Transforms::objectSpace(nv1, parent);
            nv2 = Transforms::objectSpace(nv2, parent);
            nv3 = Transforms::objectSpace(nv3, parent);

            nv1 = Transforms::cameraSpace(nv1, cam);
            nv2 = Transforms::cameraSpace(nv2, cam);
            nv3 = Transforms::cameraSpace(nv3, cam);

            nv1 = Transforms::homogeneousClipSpace(nv1, cam);
            nv2 = Transforms::homogeneousClipSpace(nv2, cam);
            nv3 = Transforms::homogeneousClipSpace(nv3, cam);

            // y obtenemos los puntos en la proyección 2d
            Point2D pnv1 = Transforms::screenSpace(nv1, cam);
            Point2D pnv2 = Transforms::screenSpace(nv2, cam);
            Point2D pnv3 = Transforms::screenSpace(nv3, cam);

            // Hayamos las coordenadas baricéntricas del punto pnv1 respecto al triángulo pa, pb, pc
            Maths::getBarycentricCoordinates(alpha, theta, gamma, pnv1.x, pnv1.y, pa, pb, pc);

            float punv1 = alpha * A.u + theta * B.u + gamma * C.u;
            float pvnv1 = alpha * A.v + theta * B.v + gamma * C.v;
            tv1.u = punv1;
            tv1.v = pvnv1;

            // Hayamos las coordenadas baricéntricas del punto pnv2 respecto al triángulo pa, pb, pc
            Maths::getBarycentricCoordinates(alpha, theta, gamma, pnv2.x, pnv2.y, pa, pb, pc);

            float punv2 = alpha * A.u + theta * B.u + gamma * C.u;
            float pvnv2 = alpha * A.v + theta * B.v + gamma * C.v;
            tv2.u = punv2;
            tv2.v = pvnv2;

            // Hayamos las coordenadas baricéntricas del punto pnv3 respecto al triángulo pa, pb, pc
            Maths::getBarycentricCoordinates(alpha, theta, gamma, pnv3.x, pnv3.y, pa, pb, pc);

            float punv3 = alpha * A.u + theta * B.u + gamma * C.u;
            float pvnv3 = alpha * A.v + theta * B.v + gamma * C.v;

            tv3.u = punv3;
            tv3.v = pvnv3;

            Triangle t = Triangle( tv1, tv2, tv3, parent );
            if (texture != NULL && EngineSetup::getInstance()->TRIANGLE_MODE_TEXTURIZED) {
                t.setTexture(texture);
                // No usamos draw() pq coinciden con los planos del frustum y
                // se autoeliminaría en el check isPointInFrustum
                // scanVertices() no realiza esos checks.
                t.updateVertexObjectSpace();
                t.updateVertexCameraSpace(cam);
                t.scanVertices(cam);
            }

            if (EngineSetup::getInstance()->TRIANGLE_MODE_WIREFRAME) {
                Vector3D v1 = Vector3D(vertexes[0], vertexes[current]);
                Vector3D v2 = Vector3D(vertexes[0], vertexes[next]);
                Vector3D v3 = Vector3D(vertexes[current], vertexes[next]);

                Drawable::drawVector3D(v1, cam, Color::red());
                Drawable::drawVector3D(v2, cam, Color::green());
                Drawable::drawVector3D(v3, cam, Color::blue());
            }

            if ( EngineSetup::getInstance()->TRIANGLE_MODE_PIXELS ) {
                Drawable::drawVertex(nv1, cam, Color::red());
                Drawable::drawVertex(nv2, cam, Color::green());
                Drawable::drawVertex(nv3, cam, Color::blue());
            }
        }

        current+=1;
    }
}

Vertex Maths::getCenterVertices(Vertex vertices[], int num_vertices) {
    Vertex middle = Vertex(0, 0, 0);

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


void Maths::sortVertexByY(Vertex &A, Vertex &B, Vertex &C)
{
    int n = 3;
    Vertex v[3];
    v[0] = A; v[1] = B; v[2] = C;

    for (int i = 1 ; i < n; i++) {
        for (int j = 0 ; j < (n - i); j++) {
            if (v[j].y > v[j+1].y) {
                Vertex aux = v[j];
                v[j] = v[j+1];
                v[j+1] = aux;
            }
        }
    }

    A = v[0]; B = v[1]; C = v[2];

    return;
}

void Maths::sortVertexByX(Vertex &A, Vertex &B, Vertex &C)
{
    int n = 3;
    Vertex v[3];
    v[0] = A; v[1] = B; v[2] = C;

    for (int i = 1 ; i < n; i++) {
        for (int j = 0 ; j < (n - i); j++) {
            if (v[j].x > v[j+1].x) {
                Vertex aux = v[j];
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

void Maths::VertexSwap(Vertex vertexes[], int i, int j)
{
    Vertex tmp = vertexes[i];
    vertexes[i] = vertexes[j];
    vertexes[j] = tmp;
}

void Maths::sortVertexesByX(Vertex vertexes[], int N)
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

void Maths::sortVertexesByAngles(Vertex vertexes[], int N)
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

void Maths::sortVertexesByY(Vertex vertexes[], int N)
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

float Maths::distanteBetweenpoints(Vertex v1, Vertex v2)
{

    float abs_vector = sqrtf( (v2.x - v1.x)*(v2.x - v1.x) + (v2.y - v1.y)*(v2.y - v1.y) + (v2.z - v1.z)*(v2.z - v1.z) );

    return abs_vector;
}

Uint32 Maths::mixColor(Uint32 color, float distance, LightPoint *lp, Vertex Q)
{

    Vertex P = lp->position;
    Vertex R = lp->forward.getUnitVector();

    Vector3D L = Vector3D(P, Q);
    Vertex Lv = L.getUnitVector();

    const float min = Vertex::dotProduct(R, Lv);

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

Vertex Maths::crossProduct(Vertex u, Vertex v)
{
    Vertex V;

    V.x = (u.y * v.z) - (u.z * v.y);
    V.y = (u.z * v.x) - (u.x * v.z);
    V.z = (u.x * v.y) - (u.y * v.x);

    return V;
}

float Maths::floatRound(double f, int c)
{
    return (((float)((int)((f) * (c))) / (c)));
}

float Maths::getHorizontalAngleBetweenObject3DAndCamera(Object3D *o1, Camera *cam)
{
    o1->updateAxis();

    Vertex oRight = o1->forward.getComponent();
    Vertex R = cam->forward.getComponent();

    float rads = acosf(  Vertex::dotProduct(R, oRight) / (R.getNorm() * oRight.getNorm()) );

    float degs = Maths::radiansToDegrees(rads);

    float dot2d = oRight.x * R.y - oRight.y * R.x;

    if (signbit(dot2d)) {
        degs =  360 - degs;
    }

    return degs;
}

