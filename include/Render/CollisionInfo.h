//
// Created by Eduardo on 06/04/2025.
//

#ifndef BRAKEZA3D_COLLISIONINFO_H
#define BRAKEZA3D_COLLISIONINFO_H


#include "../BrakezaSetup.h"
class Object3D;

class CollisionInfo {

public:
    CollisionInfo(void *with, int source, int boneIndexMapping);

    void *with;
    int source;
    int boneIndexMapping;

    int getSource() const;

    int getBoneIndexMapping() const;

    Object3D* getObject() const; // solo forward declaration es suficiente

};


#endif //BRAKEZA3D_COLLISIONINFO_H
