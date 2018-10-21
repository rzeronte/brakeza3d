//
// Created by darkhead on 28/4/18.
//

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
    Logging::getInstance()->Log(label + ": (x:" + std::to_string(x) + ", y:" + std::to_string(y) + ", z:" +std::to_string(z) + ")", "VERTEX");
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

void Vertex3D::setOrigin() {
    this->x = 0;
    this->y = 0;
    this->z = 0;

    this->u = 0;
    this->v = 0;
}

void Vertex3D::addVertex(Vertex3D v)
{
    this->x += v.x;
    this->y += v.y;
    this->z += v.z;
}

void Vertex3D::subVertex(Vertex3D v)
{
    this->x -= v.x;
    this->y -= v.y;
    this->z -= v.z;
}

float Vertex3D::dotProduct(Vertex3D v1, Vertex3D v2)
{
    float dot = (v1.x*v2.x) + (v1.y*v2.y) + (v1.z*v2.z);

    return dot;
}

float Vertex3D::getNorm()
{
    float norm = sqrt( (this->x*this->x) + (this->y*this->y) + (this->z*this->z) );

    return norm;
}