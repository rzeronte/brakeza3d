
#include <cmath>
#include "../../include/Objects/Vertex3D.h"
#include "../../include/Misc/Tools.h"
#include "../../include/Render/Logging.h"
#include "../../include/Render/Maths.h"

Vertex3D::Vertex3D() {
    this->x = 0;
    this->y = 0;
    this->z = 0;
}

Vertex3D::Vertex3D(float x, float y, float z) {
    this->x = x;
    this->y = y;
    this->z = z;
}

Vertex3D::Vertex3D(const float v[3]) {
    this->x = v[0];
    this->y = v[1];
    this->z = v[2];
}

Vertex3D::Vertex3D(Vertex4D v) {
    this->x = v.x / v.w;
    this->y = v.y / v.w;
    this->z = v.z / v.w;
}

Vertex3D Vertex3D::operator+(const Vertex3D &v) const {
    return Vertex3D(
        this->x + v.x,
        this->y + v.y,
        this->z + v.z
    );
}

Vertex3D Vertex3D::operator-(const Vertex3D &v) const {
    return Vertex3D(
        this->x - v.x,
        this->y - v.y,
        this->z - v.z
    );
}

Vertex3D Vertex3D::operator%(const Vertex3D &v) const {
    return Vertex3D(
            (this->y * v.z) - (this->z * v.y),
            (this->z * v.x) - (this->x * v.z),
            (this->x * v.y) - (this->y * v.x)
    );
}

float Vertex3D::operator*(const Vertex3D &v) const {
    return (this->x * v.x) + (this->y * v.y) + (this->z * v.z);
}

bool Vertex3D::operator==(const Vertex3D &v) const {
    if (this->x == v.x && this->y == v.y && this->z == v.z) {
        return true;
    }

    return false;
}

bool Vertex3D::operator!=(const Vertex3D &v) const {
    if (this->x == v.x && this->y == v.y && this->z == v.z) {
        return false;
    }

    return true;
}

Vertex3D Vertex3D::getNormalize() const {
    float modulo = abs(Maths::sqrt1((this->x * this->x) + (this->y * this->y) + (this->z * this->z)));

    float inv_module = 1 / modulo;

    Vertex3D C(
            this->x * inv_module,
            this->y * inv_module,
            this->z * inv_module
    );

    C.u = this->u;
    C.v = this->v;

    return C;
}

Vertex3D Vertex3D::zero() {
    return Vertex3D(0, 0, 0);
}

void Vertex3D::consoleInfo(const std::string& label, bool returnLine) const {
    Logging::Log(
            label + ": (x:" + std::to_string(x) + ", y:" + std::to_string(y) + ", z: " + std::to_string(z) + ")",
            "VERTEX");
    if (returnLine) {
        Logging::Log("", "VERTEX");
    }
}

Vertex3D Vertex3D::getInverse() const {
    Vertex3D t(-this->x,-this->y,-this->z);

    t.u = this->u;
    t.v = this->v;

    return t;
}

Vertex3D Vertex3D::getAbsolute() const {
    Vertex3D t;

    t.x = abs(this->x);
    t.y = abs(this->y);
    t.z = abs(this->z);

    t.u = this->u;
    t.v = this->v;

    return t;
}

float Vertex3D::getModule() const {
    float norm = sqrt((this->x * this->x) + (this->y * this->y) + (this->z * this->z));

    return norm;
}

float Vertex3D::getSquaredLength() const {
    float norm = (this->x * this->x) + (this->y * this->y) + (this->z * this->z);

    return norm;
}

void Vertex3D::setScaled(float s) {
    this->x *= s;
    this->y *= s;
    this->z *= s;
}

Vertex3D Vertex3D::getScaled(float s) const {
    Vertex3D v;

    v.x = this->x * s;
    v.y = this->y * s;
    v.z = this->z * s;

    return v;
}

Vertex3D Vertex3D::getScaled(float xs, float ys, float zs) const {
    Vertex3D v;

    v.x = this->x * xs;
    v.y = this->y * ys;
    v.z = this->z * zs;

    return v;
}

void Vertex3D::setLength(float length) {
    Vertex3D n = this->getNormalize();

    this->x = n.x * length;
    this->y = n.y * length;
    this->z = n.z * length;
}

float Vertex3D::distance(Vertex3D to) {
    Vector3D tmpV(*this, to);

    return tmpV.getComponent().getModule();
}

void Vertex3D::saveToFloat3(float *v) const {
    v[0] = this->x;
    v[1] = this->y;
    v[2] = this->z;
}

void Vertex3D::saveToBtVector3(btVector3 *v) const {
    v->setX(this->x);
    v->setY(this->y);
    v->setZ(this->z);
}

Vertex4D Vertex3D::createVertex4D() {
    return Vertex4D(
        this->x,
        this->y,
        this->z,
        1
    );
}

