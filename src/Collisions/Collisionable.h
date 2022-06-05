#ifndef BRAKEDA3D_COLLISIONABLE_H
#define BRAKEDA3D_COLLISIONABLE_H


#include "../../include/Objects/Object3D.h"

class Collisionable {
private:
    bool collisionsEnabled;
    float mass;
public:

    Collisionable();

    virtual void resolveCollision(Collisionable *collisionable) = 0;
    virtual void integrate();
    bool isCollisionsEnabled();
    void setCollisionsEnabled(bool value);
};


#endif //BRAKEDA3D_COLLISIONABLE_H
