//
// Created by eduardo on 8/01/23.
//

#ifndef BRAKEZA3D_RAYLIGHT_H
#define BRAKEZA3D_RAYLIGHT_H

#include <BulletCollision/CollisionDispatch/btCollisionWorld.h>
#include "../../include/Render/ShaderOpenCL.h"
#include "../../include/Misc/Image.h"
#include "../../include/Objects/Object3D.h"

class RayLight {
private:
    bool enabled;

    float intensity;
    float reach;
    float speed;
    float damage;

    Color color;
    Object3D* parent;

    Vertex3D direction;
    Vertex3D startOffset;

    btCollisionWorld::ClosestRayResultCallback *rayCallback;
public:
    explicit RayLight(bool enabled, Object3D *parent, Vertex3D direction, Vertex3D startOffset, float speed, float damage, Color c, int filterGroup, int filterMask);

    void update();

    void updateDirection(Vertex3D direction, Vertex3D startOffset);

    void setIntensity(float intensity);

    void resetReach();

    void setDamage(float damage);

    void increaseReach();

    [[nodiscard]] bool isEnabled() const;

    void setEnabled(bool enabled);

    void setColor(const Color &color);

    [[nodiscard]] const Vertex3D &getDirection() const;

    void setDirection(Vertex3D direction);

    Object3D *getParent() const;

    void setReach(int i);
};


#endif //BRAKEZA3D_RAYLIGHT_H
