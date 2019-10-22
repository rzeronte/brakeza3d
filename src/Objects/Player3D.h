
#ifndef BRAKEZA3D_PLAYER3D_H
#define BRAKEZA3D_PLAYER3D_H


#include "../../headers/Objects/Camera3D.h"

class Player3D : public Camera3D {
public:
    Player3D();

    int stamina;
    int lives;
};


#endif //BRAKEZA3D_PLAYER3D_H
