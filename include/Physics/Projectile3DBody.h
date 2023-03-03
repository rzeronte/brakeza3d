
#ifndef BRAKEDA3D_PROJECTILE3DBODY_H
#define BRAKEDA3D_PROJECTILE3DBODY_H


#include "Mesh3DBody.h"
#include "../Misc/Counter.h"
#include "Projectile.h"

class Projectile3DBody : public Projectile, public Mesh3DBody {
public:
    explicit Projectile3DBody(const Vertex3D &direction);

    void makeProjectileRigidBody(float mass, Vertex3D size, Vertex3D direction, M3 rotation, float forceImpulse, float accuracy, btDiscreteDynamicsWorld *world, int collisionGroup, int collisionMask);

    void onUpdate() override;

    void resolveCollision(Collisionable *objectWithCollision) override;
};


#endif //BRAKEDA3D_PROJECTILE3DBODY_H
