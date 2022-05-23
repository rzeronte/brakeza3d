//
// Created by eduardo on 12/05/22.
//

#include "Vector2D.h"
#include "../Render/Maths.h"


Vector2D::Vector2D() {
    x = 0;
    y = 0;
};

Vector2D::Vector2D(float x, float y) {
    this->x = x;
    this->y = y;
}

void Vector2D::consoleInfo(const std::string& label, bool jump) const {

    std::cout << "Vector2D " << label << ": " << x << ", " << y << " | ";

    if (jump) {
        std::cout << std::endl;
    }
}


float Vector2D::operator*(const Vector2D &v) const
{
    return (float) ((this->x * v.x) + (this->y * v.y));
}

Vector2D Vector2D::operator/(const Vector2D &pm) const {
    return Vector2D(
        this->x / pm.x,
        this->y / pm.y
    );
}

Vector2D Vector2D::getNormalize() const
{
    const float modulo = abs(Maths::sqrt1((this->x * this->x) + (this->y * this->y)));

    float inv_module = 1 / modulo;

    return Vector2D(
            this->x * inv_module,
            this->y * inv_module
    );
}

Vector2D Vector2D::operator-(const Vector2D &v) const {
    return Vector2D(
            this->x - v.x,
            this->y - v.y
    );
}

Vector2D Vector2D::operator+(const Vector2D &v) const {
    return Vector2D(
            this->x + v.x,
            this->y + v.y
    );
}

float Vector2D::getLength()
{
    return Maths::sqrt1(x * x + y*y);
}

Vector2D Vector2D::getScaled(float v) const
{
    return Vector2D(
            this->x * v,
            this->y * v
    );
}
