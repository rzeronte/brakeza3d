//
// Created by eduardo on 8/01/23.
//

#ifndef BRAKEZA3D_RAYLIGHT_H
#define BRAKEZA3D_RAYLIGHT_H

#include <BulletCollision/CollisionDispatch/btCollisionWorld.h>
#include "../../../include/Render/ShaderOpenCL.h"
#include "../../../include/Misc/Image.h"
#include "../../../include/Objects/Object3D.h"

class RayLight {
private:
    bool enabled;

    float intensity;
    float reach;
    float speed;
    float damage;

    Color color;
    Object3D* parent;

    btCollisionWorld::ClosestRayResultCallback *rayCallback;
public:
    explicit RayLight(Object3D *parent, float speed, float damage, Color c, int filterGroup, int filterMask);

    void update();

    void setIntensity(float intensity);

    void resetReach();

    void setDamage(float damage);

    void increaseReach();

    [[nodiscard]] bool isEnabled() const;

    void setEnabled(bool enabled);

    void setColor(const Color &color);
};


#endif //BRAKEZA3D_RAYLIGHT_H
