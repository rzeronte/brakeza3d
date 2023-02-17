
#ifndef BRAKEDA3D_PROJECTILE3DBODY_H
#define BRAKEDA3D_PROJECTILE3DBODY_H


#include "Mesh3DBody.h"
#include "../Misc/Counter.h"
#include "Projectile.h"

class Projectile3DBody : public Projectile, public Mesh3DBody {
public:
    Projectile3DBody(float ttl, const Vertex3D &direction);

    void makeProjectileRigidBody(float mass, Vertex3D size, Vertex3D direction, M3 rotation, float forceImpulse, float accuracy, btDiscreteDynamicsWorld *world, int collisionGroup, int collisionMask);

    virtual void onUpdate() override;

    void resolveCollision(Collisionable *collisionable) override;
};


#endif //BRAKEDA3D_PROJECTILE3DBODY_H
