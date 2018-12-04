
#include <cstdio>
#include <cmath>
#include "../../headers/Objects/Vertex3D.h"
#include "../../headers/Objects/Point2D.h"
#include "../../headers/Render/Tools.h"
#include "../../headers/Render/Color.h"
#include "../../headers/Render/Logging.h"

Vertex3D::Vertex3D()
{
    this->x = 0;
    this->y = 0;
    this->z = 0;

    this->u = 0;
    this->v = 0;
}

Vertex3D::Vertex3D(float x, float y, float z)
{
    this->x = x;
    this->y = y;
    this->z = z;

    this->u = 0;
    this->v = 0;
}

Vertex3D Vertex3D::operator +(const Vertex3D &v)
{
    Vertex3D V = Vertex3D();

    V.x = this->x + v.x;
    V.y = this->y + v.y;
    V.z = this->z + v.z;

    return V;
}

Vertex3D Vertex3D::operator -(const Vertex3D &v)
{
    Vertex3D V = Vertex3D();

    V.x = this->x - v.x;
    V.y = this->y - v.y;
    V.z = this->z - v.z;

    return V;
}

Vertex3D Vertex3D::operator %(const Vertex3D &v)
{
    Vertex3D V;

    V.x = (this->y * v.z) - (this->z * v.y);
    V.y = (this->z * v.x) - (this->x * v.z);
    V.z = (this->x * v.y) - (this->y * v.x);

    return V;
}

float Vertex3D::operator *(const Vertex3D &v)
{
    float dot = (this->x * v.x) + (this->y * v.y) + (this->z * v.z);

    return dot;
}


Vertex3D Vertex3D::getNormalize()
{
    float modulo = abs(sqrt( (this->x*this->x) + (this->y*this->y) + (this->z*this->z) ) );

    Vertex3D C(
        this->x / modulo,
        this->y / modulo,
        this->z / modulo
    );

    C.u = this->u;
    C.v = this->v;

    return C;
}

void Vertex3D::consoleInfo(std::string label, bool returnLine)
{
    Logging::getInstance()->Log(label + ": (x:" + std::to_string(x) + ", y:" + std::to_string(y) + ", z:" + std::to_string(z) + ") (u:" + std::to_string(u) + ", v:" + std::to_string(v) + ")", "VERTEX");
    if (returnLine) {
        Logging::getInstance()->Log("", "VERTEX");
    }
}

Vertex3D Vertex3D::getInverse() {
    Vertex3D t = Vertex3D();
    t.x = -this->x;
    t.y = -this->y;
    t.z = -this->z;

    t.u = this->u;
    t.v = this->v;

    return t;
}

float Vertex3D::dotProduct(Vertex3D v1, Vertex3D v2)
{
    float dot = (v1.x*v2.x) + (v1.y*v2.y) + (v1.z*v2.z);

    return dot;
}

float Vertex3D::getModule()
{
    float norm = sqrt( (this->x*this->x) + (this->y*this->y) + (this->z*this->z) );

    return norm;
}