//
// Created by eduardo on 12/05/22.
//

#include "../../include/Render/Vector2D.h"
#include "../../include/Misc/ToolsMaths.h"


Vector2D::Vector2D() {
}

Vector2D::Vector2D(float x, float y): x(x), y(y)
{
}

void Vector2D::consoleInfo(const std::string& label, bool jump) const
{

    std::cout << "Vector2D " << label << ": " << x << ", " << y << " | ";

    if (jump) {
        std::cout << std::endl;
    }
}

float Vector2D::operator*(const Vector2D &v) const
{
    return (this->x * v.x) + (this->y * v.y);
}

Vector2D Vector2D::operator/(const Vector2D &pm) const
{
    return Vector2D(
        x / pm.x,
        y / pm.y
    );
}

Vector2D Vector2D::getNormalize() const
{
    const float modulo = abs(ToolsMaths::sqrt1((x * x) + (y * y)));

    float inv_module = 1 / modulo;

    return Vector2D(
        x * inv_module,
        y * inv_module
    );
}

Vector2D Vector2D::operator-(const Vector2D &v) const
{
    return Vector2D(
        x - v.x,
        y - v.y
    );
}

Vector2D Vector2D::operator+(const Vector2D &v) const {
    return Vector2D(
        x + v.x,
        y + v.y
    );
}

float Vector2D::getLength() const
{
    return ToolsMaths::sqrt1(x * x + y*y);
}

Vector2D Vector2D::getScaled(float v) const
{
    return Vector2D(
        x * v,
        y * v
    );
}
