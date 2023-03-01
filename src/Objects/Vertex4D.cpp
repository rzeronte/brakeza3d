#include "../../include/Objects/Vertex4D.h"
#include "../../include/Render/Logging.h"

Vertex4D::Vertex4D() {
    this->x = 0;
    this->y = 0;
    this->z = 0;
    this->w = 0;
}

Vertex4D::Vertex4D(float x, float y, float z, float w) {
    this->x = x;
    this->y = y;
    this->z = z;
    this->w = w;
}

