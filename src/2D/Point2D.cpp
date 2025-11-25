#include <iostream>
#include "../../include/Render/Point2D.h"
#include "../../include/Render/Maths.h"

Point2D::Point2D(): x(0), y(0)
{
}

Point2D::Point2D(int x, int y) : x(x), y(y)
{
}

Point2D::Point2D(glm::vec2 v) : x(v.x), y(v.y)
{
}

float Point2D::operator*(const Point2D &v) const
{
    return x * v.x + y * v.y;
}

Point2D Point2D::operator/(const Point2D &pm) const
{
    return Point2D(
        x / pm.x,
        y / pm.y
    );
}

Point2D Point2D::getNormalize() const
{
    const float modulo = abs(Maths::sqrt1((this->x * this->x) + (this->y * this->y)));

    float inv_module = 1 / modulo;

    return Point2D(
        x * inv_module,
        y * inv_module
    );
}

Point2D Point2D::operator-(const Point2D &v) const
{
    return Point2D(
        x - v.x,
        y - v.y
    );
}

Point2D Point2D::operator+(const Point2D &v) const
{
    return Point2D(
        x + v.x,
        y + v.y
    );
}

Point2D Point2D::getScaled(float v) const
{
    return Point2D(
        x * v,
        y * v
    );
}

glm::vec2 Point2D::toGLM() const
{
    return glm::vec2(x, y);
}