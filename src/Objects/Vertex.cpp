//
// Created by darkhead on 28/4/18.
//

#include <cstdio>
#include <cmath>
#include "../../headers/Vertex.h"
#include "../../headers/Point2D.h"
#include "../../headers/Tools.h"
#include "../../headers/Color.h"
#include "../../headers/Logging.h"

Vertex::Vertex()
{
    this->x = 0;
    this->y = 0;
    this->z = 0;

    this->u = 0;
    this->v = 0;
}

Vertex::Vertex(float x, float y, float z)
{
    this->x = x;
    this->y = y;
    this->z = z;

    this->u = 0;
    this->v = 0;
}

Vertex Vertex::getNormalize()
{
    float modulo = abs(sqrt( (this->x*this->x) + (this->y*this->y) + (this->z*this->z) ) );

    Vertex C(
        this->x / modulo,
        this->y / modulo,
        this->z / modulo
    );

    C.u = this->u;
    C.v = this->v;

    return C;
}

void Vertex::consoleInfo(std::string label, bool returnLine)
{
    Logging::getInstance()->Log(label + ": (x:" + std::to_string(x) + ", y:" + std::to_string(y) + ", z:" +std::to_string(z) + ")", "VERTEX");
    if (returnLine) {
        Logging::getInstance()->Log("", "VERTEX");
    }
}

Vertex Vertex::getInverse() {
    Vertex t = Vertex();
    t.x = -this->x;
    t.y = -this->y;
    t.z = -this->z;

    t.u = this->u;
    t.v = this->v;

    return t;
}

void Vertex::setOrigin() {
    this->x = 0;
    this->y = 0;
    this->z = 0;

    this->u = 0;
    this->v = 0;
}

void Vertex::addVertex(Vertex v)
{
    this->x += v.x;
    this->y += v.y;
    this->z += v.z;
}

void Vertex::subVertex(Vertex v)
{
    this->x -= v.x;
    this->y -= v.y;
    this->z -= v.z;
}

float Vertex::dotProduct(Vertex v1, Vertex v2)
{
    float dot = (v1.x*v2.x) + (v1.y*v2.y) + (v1.z*v2.z);

    return dot;
}

float Vertex::getNorm()
{
    float norm = sqrt( (this->x*this->x) + (this->y*this->y) + (this->z*this->z) );

    return norm;
}