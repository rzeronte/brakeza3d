#include "../../headers/Objects/Line2D.h"

Line2D::Line2D() {
    this->x1 = 0;
    this->y1 = 0;
    this->x2 = 0;
    this->y2 = 0;
}

Line2D::Line2D(int x1, int y1, int x2, int y2) {
    this->x1 = x1;
    this->y1 = y1;
    this->x2 = x2;
    this->y2 = y2;
}

Point2D Line2D::middlePoint() const {
    return Point2D((float) (x1 + x2) / 2, (float) (y1 + y2) / 2);
}