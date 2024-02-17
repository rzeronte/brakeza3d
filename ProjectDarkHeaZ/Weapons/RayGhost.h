//
// Created by edu on 6/02/24.
//

#ifndef BRAKEZA3D_RAYGHOST_H
#define BRAKEZA3D_RAYGHOST_H


#include "../../include/Physics/Ghost.h"

class RayGhost : public Object3D, public Ghost {
    Object3D* targetOne;
    Object3D* targetTwo;

    Color color;

    float damage;
    int filterGroup;
    int filterMask;
public:

    RayGhost(float damage, Color c, Object3D *targetOne, Object3D *targetTwo, int filterGroup, int filterMask);

    void updateShape();

    void resolveCollision(Collisionable *collisionable) override;

    void updateBulletFromMesh3D();

    void onUpdate() override;

    static btCollisionShape* CustomShape(const btVector3 &startPoint, const btVector3 &endPoint, float width, float height);

    void drawImGuiProperties() override;

    [[nodiscard]] const Vertex3D &getTargetOne() const;

    [[nodiscard]] const Vertex3D &getTargetTwo() const;

    [[nodiscard]] float getDamage() const;

    [[nodiscard]] const Color &getColor() const;
};


#endif //BRAKEZA3D_RAYGHOST_H
