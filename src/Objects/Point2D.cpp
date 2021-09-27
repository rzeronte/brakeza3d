//
// Created by darkhead on 28/4/18.
//

#include <iostream>
#include "../../headers/Objects/Point2D.h"

Point2D::Point2D() {
}

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