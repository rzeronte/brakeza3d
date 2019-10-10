//
// Created by darkhead on 30/5/18.
//

#include "../../headers/Render/Plane.h"
#include "../../headers/Render/Tools.h"
#include "../../headers/Render/Maths.h"

Plane::Plane() {}

Plane::Plane(Vertex3D A, Vertex3D B, Vertex3D C)
{
    this->A = A;
    this->B = B;
    this->C = C;

    updateNormalVector();
}

Plane::Plane(const Vertex3D P, const Vertex3D N)
{
    this->normal = N;
    this->A = P;
}

void Plane::setOrigin(Vertex3D origin)
{
    this->A = origin;
}

void Plane::setNormal(Vertex3D n)
{
    this->normal = n;
}

float Plane::distance(const Vertex3D &p)
{
    Vertex3D n = getNormalVector().getNormalize();

    float D = - ( (n.x * A.x) + (n.y * A.y) + (n.z * A.z) );
    float dist = ( (n.x * p.x) + (n.y * p.y) + (n.z * p.z) + D);

    return dist;
}

void Plane::updateNormalVector()
{
    Vector3D VnAB(this->A, this->B);
    Vector3D VnAC(this->A, this->C);

    Vertex3D U = VnAB.getComponent();
    Vertex3D V = VnAC.getComponent();

    normal = U % V;
}

Vertex3D Plane::getNormalVector()
{
    return this->normal;
}

Vertex3D Plane::getPointIntersection(Vertex3D vertex1, Vertex3D vertex2, float &transition)
{

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
        vertex1.x + t * ( a ),
        vertex1.y + t * ( b ),
        vertex1.z + t * ( c )
    );

    return P;
}

bool Plane::isFrontFacingTo(Vertex3D direction)
{
    double dot = this->normal * direction;
    return (dot <= 0);
}

bool Plane::intersect(Vector3D ray, float &t)
{
    if (Maths::isVector3DClippingPlane(*this, ray)) {
        this->getPointIntersection(ray.origin(), ray.end(), t);

        return true;
    }

    return false;
}

Vertex3D Plane::origin() const
{
    return this->A;
}

Vertex3D Plane::closest( Vertex3D p )
{
    float t = this->distance(p);

    return p - this->getNormalVector().getNormalize().getScaled( t );
}