#include "Rotation3D.h"
#include "../../headers/Render/Logging.h"

Rotation3D::Rotation3D() {
    this->x = 0;
    this->y = 0;
    this->z = 0;
}

Rotation3D::Rotation3D(float x, float y, float z) : x(x), y(y), z(z) {

}

void Rotation3D::addRotation(Rotation3D r)
{
    this->x += r.x;
    this->y += r.y;
    this->z += r.z;
}


void Rotation3D::consoleInfo(std::string label, bool returnLine)
{
    Logging::getInstance()->Log(label + ": (x:" + std::to_string(x) + ", y:" + std::to_string(y) + ", z:" +std::to_string(z) + ")", "Rotation3D");
    if (returnLine) {
        Logging::getInstance()->Log("", "Rotation3D");
    }
}