
#include <cmath>
#include "../../headers/Objects/Vertex3D.h"
#include "../../headers/Misc/Tools.h"
#include "../../headers/Render/Logging.h"

Vertex3D::Vertex3D()
{
}

Vertex3D::Vertex3D(float x, float y, float z)
{
    this->x = x;
    this->y = y;
    this->z = z;
}

Vertex3D::Vertex3D(float v[3])
{
    this->x = v[0];
    this->y = v[1];
    this->z = v[2];
}

Vertex3D Vertex3D::operator +(const Vertex3D &v)
{
    return Vertex3D(
        this->x + v.x,
        this->y + v.y,
        this->z + v.z
    );
}

Vertex3D Vertex3D::operator -(const Vertex3D &v)
{
    return Vertex3D(
        this->x - v.x,
        this->y - v.y,
        this->z - v.z
    );
}

Vertex3D Vertex3D::operator %(const Vertex3D &v)
{
    return Vertex3D(
        (this->y * v.z) - (this->z * v.y),
        (this->z * v.x) - (this->x * v.z),
        (this->x * v.y) - (this->y * v.x)
    );
}

float Vertex3D::operator *(const Vertex3D &v)
{
    return (this->x * v.x) + (this->y * v.y) + (this->z * v.z);
}

bool Vertex3D::operator ==(const Vertex3D &v)
{
    if (this->x == v.x && this->y == v.y && this->z == v.z) {
        return true;
    }

    return false;
}

bool Vertex3D::operator !=(const Vertex3D &v)
{
    if (this->x == v.x && this->y == v.y && this->z == v.z) {
        return false;
    }

    return true;
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

Vertex3D Vertex3D::zero()
{
    return Vertex3D(0, 0, 0);
}

void Vertex3D::consoleInfo(std::string label, bool returnLine)
{
    Logging::getInstance()->Log(label + ": (x:" + std::to_string(x) + ", y:" + std::to_string(y) + ", z:" + std::to_string(z) + ")", "VERTEX");
    if (returnLine) {
        Logging::getInstance()->Log("", "VERTEX");
    }
}

Vertex3D Vertex3D::getInverse()
{
    Vertex3D t;

    t.x = -this->x;
    t.y = -this->y;
    t.z = -this->z;

    t.u = this->u;
    t.v = this->v;

    return t;
}

Vertex3D Vertex3D::getAbsolute()
{
    Vertex3D t;

    t.x = abs(this->x);
    t.y = abs(this->y);
    t.z = abs(this->z);

    t.u = this->u;
    t.v = this->v;

    return t;
}

float Vertex3D::getModule()
{
    float norm = sqrt( (this->x*this->x) + (this->y*this->y) + (this->z*this->z) );

    return norm;
}

float Vertex3D::squaredLength()
{
    float norm = (this->x*this->x) + (this->y*this->y) + (this->z*this->z);

    return norm;
}

Vertex3D Vertex3D::setScaled(float s)
{
    this->x*=s;
    this->y*=s;
    this->z*=s;
}

Vertex3D Vertex3D::getScaled(float s)
{
    Vertex3D v;

    v.x = this->x * s;
    v.y = this->y * s;
    v.z = this->z * s;

    return v;
}

Vertex3D Vertex3D::getScaled(float xs, float ys, float zs)
{
    Vertex3D v;

    v.x = this->x * xs;
    v.y = this->y * ys;
    v.z = this->z * zs;

    return v;
}

void Vertex3D::setLength(float length)
{
    Vertex3D n = this->getNormalize();

    this->x = n.x * length;
    this->y = n.y * length;
    this->z = n.z * length;
}

float Vertex3D::distance(Vertex3D to)
{
    Vector3D tmpV(*this, to);

    return tmpV.getComponent().getModule();
}

void Vertex3D::saveToFloat3(float *v)
{
    v[0] = this->x;
    v[1] = this->y;
    v[2] = this->z;
}

void Vertex3D::saveToBtVector3(btVector3 *v)
{
    v->setX(this->x);
    v->setY(this->y);
    v->setZ(this->z);
}