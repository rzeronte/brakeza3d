#include <iostream>
#include "../../include/Objects/Point2D.h"
#include "../../include/Render/Maths.h"

Point2D::Point2D() {
    x = 0;
    y = 0;
};

Point2D::Point2D(int x, int y) {
    this->x = x;
    this->y = y;
}

void Point2D::consoleInfo(const std::string& label, bool jump) const {

    std::cout << "Point2D " << label << ": " << x << ", " << y << " | ";

    if (jump) {
        std::cout << std::endl;
    }
}


float Point2D::operator*(const Point2D &v) const
{
    return (float) ((this->x * v.x) + (this->y * v.y));
}

Point2D Point2D::getNormalize() const
{
    const float modulo = abs(Maths::sqrt1((this->x * this->x) + (this->y * this->y)));

    float inv_module = 1 / modulo;

    return Point2D(
        this->x * inv_module,
        this->y * inv_module
    );
}

Point2D Point2D::operator-(const Point2D &v) const {
    return Point2D(
        this->x - v.x,
        this->y - v.y
    );
}

Point2D Point2D::operator+(const Point2D &v) const {
    return Point2D(
        this->x + v.x,
        this->y + v.y
    );
}

float Point2D::getLength()
{
    return Maths::sqrt1(x * x + y*y);
}

Point2D Point2D::getScaled(float v) const
{
    return Point2D(
        this->x * v,
        this->y * v
    );
}
