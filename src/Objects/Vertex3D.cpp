
#include <cmath>
#include "../../include/Objects/Vertex3D.h"
#include "../../include/Misc/Tools.h"
#include "../../include/Render/Maths.h"

Vertex3D::Vertex3D() : x(0), y(0), z(0), u(0), v(0)
{
}

Vertex3D::Vertex3D(float x, float y, float z) : x(x), y(y), z(z), u(0), v(0)
{
}

Vertex3D::Vertex3D(const float v[3]) : x(v[0]), y(v[1]), z(v[2]), u(0), v(0)
{
}

Vertex3D Vertex3D::operator+(const Vertex3D &v) const
{
    return {
        this->x + v.x,
        this->y + v.y,
        this->z + v.z
    };
}

Vertex3D Vertex3D::operator-(const Vertex3D &v) const
{
    return {
        this->x - v.x,
        this->y - v.y,
        this->z - v.z
    };
}

Vertex3D Vertex3D::operator%(const Vertex3D &v) const
{
    return {
        (this->y * v.z) - (this->z * v.y),
        (this->z * v.x) - (this->x * v.z),
        (this->x * v.y) - (this->y * v.x)
    };
}

float Vertex3D::operator*(const Vertex3D &v) const
{
    return (this->x * v.x) + (this->y * v.y) + (this->z * v.z);
}

bool Vertex3D::operator==(const Vertex3D &v) const
{
    if (this->x == v.x && this->y == v.y && this->z == v.z) {
        return true;
    }

    return false;
}

bool Vertex3D::operator!=(const Vertex3D &v) const
{
    if (this->x == v.x && this->y == v.y && this->z == v.z) {
        return false;
    }

    return true;
}

Vertex3D Vertex3D::getNormalize() const
{
    const float modulo = abs(Maths::sqrt1((this->x * this->x) + (this->y * this->y) + (this->z * this->z)));
    const float inv_module = 1 / modulo;
    Vertex3D C = Vertex3D(x, y, z).getScaled(inv_module);
    C.u = this->u;
    C.v = this->v;

    return C;
}

Vertex3D Vertex3D::zero()
{
    return {0, 0, 0};
}

Vertex3D Vertex3D::getInverse() const {
    Vertex3D t(-this->x,-this->y,-this->z);

    t.u = this->u;
    t.v = this->v;

    return t;
}

float Vertex3D::getModule() const
{
    return sqrt((this->x * this->x) + (this->y * this->y) + (this->z * this->z));
}

float Vertex3D::getSquaredLength() const
{
    float norm = (this->x * this->x) + (this->y * this->y) + (this->z * this->z);

    return norm;
}

void Vertex3D::setScaled(float s)
{
    this->x *= s;
    this->y *= s;
    this->z *= s;
}

Vertex3D Vertex3D::getScaled(float s) const
{
    Vertex3D v;

    v.x = this->x * s;
    v.y = this->y * s;
    v.z = this->z * s;

    return v;
}

Vertex3D Vertex3D::getScaled(float xs, float ys, float zs) const
{
    Vertex3D v;

    v.x = this->x * xs;
    v.y = this->y * ys;
    v.z = this->z * zs;

    return v;
}

float Vertex3D::distance(Vertex3D to)
{
    Vector3D tmpV(*this, to);

    return tmpV.getComponent().getModule();
}

void Vertex3D::saveToBtVector3(btVector3 *v) const
{
    v->setX(this->x);
    v->setY(this->y);
    v->setZ(this->z);
}

Vertex3D Vertex3D::divide(float value) const
{
    Vertex3D v;

    v.x = this->x / value;
    v.y = this->y / value;
    v.z = this->z / value;

    return v;
}

glm::vec3 Vertex3D::toGLM() const
{
    return {x, y, z};
}

btVector3 Vertex3D::toBullet() const
{
    return {x, y, z};
}

Vertex3D Vertex3D::fromBullet(btVector3 &v)
{
    return {v.x(), v.y(), v.z()};
}