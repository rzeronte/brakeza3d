
#include "../headers/Brakeza3D.h"

Brakeza3D* Brakeza3D::instance = 0;

Brakeza3D::Brakeza3D() {

}

Brakeza3D* Brakeza3D::getInstance()
{
    if (instance == 0) {
        instance = new Brakeza3D();
    }

    return instance;
}