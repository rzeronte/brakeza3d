
#ifndef BRAKEDA3D_PROJECTILE3DBODY_H
#define BRAKEDA3D_PROJECTILE3DBODY_H


#include "Mesh3DBody.h"
#include "../Misc/Counter.h"

class Projectile3DBody : public Mesh3DBody {
private:
    Counter timeToLive;
    float ttl;

public:
    Projectile3DBody();

    void makeProjectileRigidBody(float mass,  Vertex3D direction, float forceImpulse, float accuracy, btDiscreteDynamicsWorld *world);
    void setTTL(float v);
    void onUpdate() override;
};


#endif //BRAKEDA3D_PROJECTILE3DBODY_H
