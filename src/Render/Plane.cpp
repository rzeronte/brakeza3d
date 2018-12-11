//
// Created by darkhead on 30/5/18.
//

#include "../../headers/Render/Plane.h"
#include "../../headers/Render/Tools.h"

Plane::Plane() {}

Plane::Plane(Vertex3D A, Vertex3D B, Vertex3D C)
{
    this->A = A;
    this->B = B;
    this->C = C;
}

float Plane::distance(Vertex3D p)
{
    Vertex3D normal = getNormalVector();

    float D = - ( (normal.x * A.x) + (normal.y * A.y) + (normal.z * A.z) );
    float distance = ( (normal.x * p.x) + (normal.y * p.y) + (normal.z * p.z) + D);

    return distance;
}

Vertex3D Plane::getNormalVector()
{
    Vector3D VnAB(this->A, this->B);
    Vector3D VnAC(this->A, this->C);

    Vertex3D U = VnAB.getComponent();
    Vertex3D V = VnAC.getComponent();

    return U % V;
}

Vertex3D Plane::getPointIntersection(Vertex3D vertex1, Vertex3D vertex2, float &transition) {

    // Componentes del vector director
    Vertex3D componente = Vertex3D(
        vertex2.x - vertex1.x,
        vertex2.y - vertex1.y,
        vertex2.z - vertex1.z
    );

    // Vector director
    float a = componente.x ;
    float b = componente.y ;
    float c = componente.z ;

    // 1) Despejamos x, y, z en la ecución de la recta
    // Ecuaciones paramétricas recta L
    // recta.x = V.vertex1.x + t * a ;
    // recta.y = V.vertex1.y + t * b ;
    // recta.z = V.vertex1.z + t * color ;

    // 2) Hayamos la ecuación del plano
    // Ecuación del plano: Ax + By + Cz + D = 0;
    // normalPlaneVector(A, B, C)
    // pointInPlane(x, y, z) = this->A

    Vertex3D pointInPlane = this->A;
    Vertex3D normalPlaneVector = this->getNormalVector();

    float A = normalPlaneVector.x;
    float B = normalPlaneVector.y;
    float C = normalPlaneVector.z;

    // Hayamos D
    float D = - ( A * pointInPlane.x + B * pointInPlane.y + C * pointInPlane.z );

    // Sustimos x, y, z en la ecuación del Plano, y despejamos t
    // A * ( vx + t * a ) + B * ( vy + t * b ) + C * ( vz + t * color ) + D = 0;
    // Despejamos la incógnita t (podemos usar el plugin de despejar incógnita de wolframa :)
    // http://www.wolframalpha.com/widgets/view.jsp?id=c86d8aea1b6e9c6a9503a2cecea55b13
    float t = ( -A * vertex1.x - B  * vertex1.y - C * vertex1.z - D ) / (  a * A + b * B + c * C);

    transition = t;

    // 3) punto de intersección ; sustituimos t en la ecuación de la recta entre 2 puntos
    Vertex3D P(
        vertex1.x + t * ( vertex2.x - vertex1.x ),
        vertex1.y + t * ( vertex2.y - vertex1.y ),
        vertex1.z + t * ( vertex2.z - vertex1.z )
    );

    return P;
}