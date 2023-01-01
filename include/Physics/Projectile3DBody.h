
#ifndef BRAKEDA3D_PROJECTILE3DBODY_H
#define BRAKEDA3D_PROJECTILE3DBODY_H


#include "Mesh3DBody.h"
#include "../Misc/Counter.h"

class Projectile3DBody : public Mesh3DBody {
private:
    Counter timeToLive;
    float ttl;

protected:
    Vertex3D direction;
public:
    Projectile3DBody();

    void makeProjectileRigidBody(float mass, Vertex3D projectileDirection, M3 rotation, float forceImpulse, float accuracy, btDiscreteDynamicsWorld *world, int collisionGroup, int collisionMask);
    void setTTL(float v);

    virtual void onUpdate() override;

    void resolveCollision(Collisionable *collisionable) override;
};


#endif //BRAKEDA3D_PROJECTILE3DBODY_H
