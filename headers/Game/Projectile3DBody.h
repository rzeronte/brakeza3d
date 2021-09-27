
#ifndef BRAKEDA3D_PROJECTILE3DBODY_H
#define BRAKEDA3D_PROJECTILE3DBODY_H


#include "../Physics/Mesh3DBody.h"

class Projectile3DBody : public Mesh3DBody {
private:
    bool fromEnemy;
    float damage;
    float damageRadius;

public:
    Projectile3DBody();

    bool isFromEnemy() const;

    void setFromEnemy(bool isFromEnemy);

    float getDamage() const;

    void setDamage(float newDamage);

    float getDamageRadius() const;

    void setDamageRadius(float newDamageRadius);

    btRigidBody *makeProjectileRigidBody(float mass, Vertex3D size, Camera3D *cam, btDiscreteDynamicsWorld *world,
                                         bool applyCameraImpulse, float forceImpulse, float accuracy);

};


#endif //BRAKEDA3D_PROJECTILE3DBODY_H
