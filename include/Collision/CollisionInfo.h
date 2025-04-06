//
// Created by Eduardo on 06/04/2025.
//

#ifndef BRAKEZA3D_COLLISIONINFO_H
#define BRAKEZA3D_COLLISIONINFO_H


#include "../EngineSetup.h"

class CollisionInfo {

public:
    CollisionInfo(void *with, int source, int boneIndexMapping);

    void *with;
    int source;
    int boneIndexMapping;
};


#endif //BRAKEZA3D_COLLISIONINFO_H
