/*
 * Brakeza3D - 3D/2D game engine
 * Author:  Eduardo Rodríguez Álvarez
 * GitHub:  https://github.com/rzeronte/brakeza3d
 * Web:     https://brakeza.com
 * Started: 2018-09-30
 * Copyright (C) 2018-2026 Eduardo Rodríguez Álvarez
 * License: GNU General Public License v3.0 (see LICENSE)
 */
//
// Created by eduardo on 19/01/23.
//

#ifndef BRAKEZA3D_PROJECTILE_H
#define BRAKEZA3D_PROJECTILE_H


#include <BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h>
#include "Vertex3D.h"
#include "../Render/Collider.h"
#include "Mesh3D.h"

class Projectile: public Mesh3D {
private:
    Vertex3D direction;
public:

    explicit Projectile(const Vertex3D &direction);

    [[nodiscard]] const Vertex3D &getDirection() const;

    void setDirection(const Vertex3D &value);

    void makeProjectileRigidBody(
        float mass,
        Vertex3D dir,
        float impulse,
        float accuracy,
        btDiscreteDynamicsWorld *world,
        CollisionShape shape,
        int collisionGroup,
        int collisionMask
    );

    void onUpdate() override;

    void ResolveCollision(CollisionInfo objectWithCollision) override;
};


#endif //BRAKEZA3D_PROJECTILE_H
