#include <iostream>
#include "../../include/Objects/Point2D.h"

Point2D::Point2D() = default;

Point2D::Point2D(float x, float y) {
    this->x = x;
    this->y = y;
}

void Point2D::consoleInfo(const std::string& label, bool jump) const {

    std::cout << "Point2D " << label << ": " << x << ", " << y << " | ";

    if (jump) {
        std::cout << std::endl;
    }
}