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

void Vertex4D::consoleInfo(const std::string& label, bool returnLine) const {
    Logging::Log(
            label + ": (x:" + std::to_string(x) + ", y:" + std::to_string(y) + ", z: " + std::to_string(z) + "), w:" + std::to_string(w),
            "VERTEX");

    if (returnLine) {
        Logging::Log("", "VERTEX");
    }
}
