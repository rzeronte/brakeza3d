#include "../headers/Render.h"
#include "../headers/Tools.h"
#include "../headers/Triangle.h"
#include "../headers/Drawable.h"
#include "../headers/EngineBuffers.h"
#include "../headers/M3.h"
#include "../headers/Logging.h"

Vertex Render::objectSpace(Vertex A, Object3D *o)
{
    Vertex v = A;

    v = Render::rotateVertex(v, *o->getRotation());
    v.addVertex(*o->getPosition());

    v.x*=o->scale;
    v.y*=o->scale;
    v.z*=o->scale;

    return v;
}

Vertex Render::cameraSpace(Vertex V, Camera *cam)
{
    Vertex A = V;

    A.subVertex(*cam->getPosition());
    A = Render::rotateVertex(A, *cam->getRotation());

    A.u = V.u; A.v = V.v;

    return A;
}

Vertex Render::homogeneousClipSpace(Vertex v, Camera *cam)
{
    Vertex vNL = cam->frustum->near_left.vertex1;
    Vertex vNR = cam->frustum->near_right.vertex1;
    if (vNL.x < vNR.x) {
        Vertex tmp = vNR;
        vNR = vNL;
        vNL = tmp;
    }

    Vertex vNT = cam->frustum->near_top.vertex1;
    Vertex vNB = cam->frustum->near_bottom.vertex1;
    if (vNT.y < vNB.y) {
        Vertex tmp = vNT;
        vNT = vNB;
        vNB = tmp;
    }

    vNL = Render::cameraSpace(vNL, cam);
    vNR = Render::cameraSpace(vNR, cam);
    vNT = Render::cameraSpace(vNT, cam);
    vNB = Render::cameraSpace(vNB, cam);

    // get 2d coordinates
    vNL = Render::screenSpacePerspective(vNL, cam);
    vNR = Render::screenSpacePerspective(vNR, cam);
    vNT = Render::screenSpacePerspective(vNT, cam);
    vNB = Render::screenSpacePerspective(vNB, cam);

    // Perspective projection ( w = 1)
    Vertex A = Render::screenSpacePerspective(v, cam);

    // HomogeneousClipSpace
    float Ax = A.x;
    float vNLx = vNL.x;
    float vNRx = vNR.x;
    float tx0 = (Ax - vNLx);
    float tx1 =  2 / ( vNRx - vNLx);
    float xt =  (tx0 * tx1)  - 1;

    float Ay = A.y;
    float vNBy = vNB.y;
    float vNTy = vNT.y;
    float ty0 = (Ay - vNBy);
    float ty1 =  2 / (vNTy - vNBy);
    float yt =  (ty0 * ty1)  - 1;

    A.x = xt;
    A.y = yt;

    A.z = v.z;      // Almaceno z (deberia ser w)

    return A;
}

Point2D Render::screenSpace(Vertex V, Camera *cam)
{
    Point2D A;
    A.x = V.x * cam->pixelPerUnit;
    A.y = V.y * cam->pixelPerUnit;

    A.x = (1 + V.x) * ((float) EngineSetup::getInstance()->SCREEN_WIDTH/2);
    A.y = (1 + V.y) * ((float) EngineSetup::getInstance()->SCREEN_HEIGHT/2);

    A.x = (int) A.x;
    A.y = (int) A.y;

    return A;
}

Vertex Render::screenSpacePerspective(Vertex v, Camera *cam)
{
    Vertex A = v;
    if (v.z != 0 || v.z != -0) {
        A.x =  - ( ( cam->frustum->nearDist * v.x) / v.z ) ;
        A.y =  - ( ( cam->frustum->nearDist * v.y) / v.z );
    }

    return A;
}


Vertex Render::objectToLocal(Vertex V, Object3D *o)
{
    Vertex T = V;

    T.x*=o->scale;
    T.y*=o->scale;
    T.z*=o->scale;

    T.subVertex(*o->getPosition());

    M3 MRX = M3::RX(-o->getRotation()->x);
    M3 MRY = M3::RY(-o->getRotation()->y);
    M3 MRZ = M3::RZ(-o->getRotation()->z);

    T = (MRZ * MRY * MRX ) * T;

    return T;
}

Vertex Render::rotateVertex(Vertex V, Vertex rotation)
{

    M3 MRX = M3::RX(rotation.x);
    M3 MRY = M3::RY(rotation.y);
    M3 MRZ = M3::RZ(rotation.z);

    Vertex B = (MRX * MRY * MRZ) * V;

    B.u = V.u; B.v = V.v;

    return B;
}

// https://elcodigografico.wordpress.com/2014/03/29/coordenadas-baricentricas-en-triangulos/
float Render::barycentricSide(int x, int y, Point2D pa, Point2D pb)
{
    return (pa.y - pb.y) * x + (pb.x - pa.x) * y + pa.x*pb.y - pb.x*pa.y;
}

void Render::getBarycentricCoordinates(float &alpha, float &theta, float &gamma, int x, int y, Point2D v1, Point2D v2,
                                       Point2D v3)
{
    alpha = Render::barycentricSide( x, y, v2, v3 ) / Render::barycentricSide( v1.x, v1.y, v2, v3 );
    theta = Render::barycentricSide( x, y, v3, v1 ) / Render::barycentricSide( v2.x, v2.y, v3, v1 );
    gamma = Render::barycentricSide( x, y, v1, v2 ) / Render::barycentricSide( v3.x, v3.y, v1, v2 );

}

bool Render::isVector3DClippingPlane(Plane P, Vector3D V)
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


bool Render::triangulate(Vertex vertexes[], int num_vertex, Object3D *parent, Camera *cam, Vertex A, Vertex B, Vertex C, Texture *texture)
{

    // Usamos un vértice arbitrario para trazar un radio
    // hacia el vértice 0
    Vertex middle = Tools::getCenterVertices(vertexes, num_vertex);
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

            angle = Tools::radiansToDegrees(angle);
        }

        vertexes[i].angle = angle;
        //std::cout << "Angle vertex " << i << " " << label << ", deg: " << angle << std::endl;
    }

    Tools::sortVertexesByAngles(vertexes, num_vertex);

    Vertex Ao = A;
    Vertex Bo = B;
    Vertex Co = C;

    Ao = Render::objectSpace(Ao, parent);
    Bo = Render::objectSpace(Bo, parent);
    Co = Render::objectSpace(Co, parent);

    // Pasamos por la cámara
    Ao = Render::cameraSpace(Ao, cam);
    Bo = Render::cameraSpace(Bo, cam);
    Co = Render::cameraSpace(Co, cam);

    Ao = Render::homogeneousClipSpace(Ao, cam);
    Bo = Render::homogeneousClipSpace(Bo, cam);
    Co = Render::homogeneousClipSpace(Co, cam);

    // y obtenemos los puntos en la proyección 2d
    Point2D pa = Render::screenSpace(Ao, cam);
    Point2D pb = Render::screenSpace(Bo, cam);
    Point2D pc = Render::screenSpace(Co, cam);

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
            Vertex tv1 = Render::objectToLocal(vertexes[0], parent);
            Vertex tv2 = Render::objectToLocal(vertexes[current], parent);
            Vertex tv3 = Render::objectToLocal(vertexes[next], parent);

            //Tools::sortVertexByX(tv1, tv2,tv3);
            Tools::sortVertexByY(tv1, tv2,tv3);

            // Vamos a calcular las coordenadas UV para tv1, tv2 y tv3
            Vertex nv1 = tv1;
            Vertex nv2 = tv2;
            Vertex nv3 = tv3;

            // Pasamos por la cámara
            nv1 = Render::objectSpace(nv1, parent);
            nv2 = Render::objectSpace(nv2, parent);
            nv3 = Render::objectSpace(nv3, parent);

            nv1 = Render::cameraSpace(nv1, cam);
            nv2 = Render::cameraSpace(nv2, cam);
            nv3 = Render::cameraSpace(nv3, cam);

            nv1 = Render::homogeneousClipSpace(nv1, cam);
            nv2 = Render::homogeneousClipSpace(nv2, cam);
            nv3 = Render::homogeneousClipSpace(nv3, cam);

            // y obtenemos los puntos en la proyección 2d
            Point2D pnv1 = Render::screenSpace(nv1, cam);
            Point2D pnv2 = Render::screenSpace(nv2, cam);
            Point2D pnv3 = Render::screenSpace(nv3, cam);

            // Hayamos las coordenadas baricéntricas del punto pnv1 respecto al triángulo pa, pb, pc
            Render::getBarycentricCoordinates(alpha, theta, gamma, pnv1.x, pnv1.y, pa, pb, pc);

            float punv1 = alpha * A.u + theta * B.u + gamma * C.u;
            float pvnv1 = alpha * A.v + theta * B.v + gamma * C.v;
            tv1.u = punv1;
            tv1.v = pvnv1;

            // Hayamos las coordenadas baricéntricas del punto pnv2 respecto al triángulo pa, pb, pc
            Render::getBarycentricCoordinates(alpha, theta, gamma, pnv2.x, pnv2.y, pa, pb, pc);

            float punv2 = alpha * A.u + theta * B.u + gamma * C.u;
            float pvnv2 = alpha * A.v + theta * B.v + gamma * C.v;
            tv2.u = punv2;
            tv2.v = pvnv2;

            // Hayamos las coordenadas baricéntricas del punto pnv3 respecto al triángulo pa, pb, pc
            Render::getBarycentricCoordinates(alpha, theta, gamma, pnv3.x, pnv3.y, pa, pb, pc);

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