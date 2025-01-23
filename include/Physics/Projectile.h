//
// Created by eduardo on 19/01/23.
//

#ifndef BRAKEZA3D_PROJECTILE_H
#define BRAKEZA3D_PROJECTILE_H


#include <BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h>
#include "../Objects/Vertex3D.h"
#include "../Render/M3.h"
#include "../Collision/Collider.h"
#include "../Objects/Mesh3D.h"

class Projectile: public Mesh3D {
private:
    Vertex3D direction;
public:

    explicit Projectile(Vertex3D direction);

    [[nodiscard]] const Vertex3D &getDirection() const;

    void setDirection(const Vertex3D &value);

    void makeProjectileRigidBody(
        float mass,
        Vertex3D direction,
        float forceImpulse,
        float accuracy,
        btDiscreteDynamicsWorld *world,
        int collisionGroup,
        int collisionMask
    );

    void onUpdate() override;

    void resolveCollision(Collider *objectWithCollision) override;
};


#endif //BRAKEZA3D_PROJECTILE_H
