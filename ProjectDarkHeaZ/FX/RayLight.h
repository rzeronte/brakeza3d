//
// Created by eduardo on 8/01/23.
//

#ifndef BRAKEZA3D_RAYLIGHT_H
#define BRAKEZA3D_RAYLIGHT_H

#include <BulletCollision/CollisionDispatch/btCollisionWorld.h>
#include "../../include/OpenGL/FXEffectOpenGL.h"
#include "../../include/Misc/Image.h"
#include "../../include/Objects/Object3D.h"
#include "../../include/Objects/LightPoint3D.h"
#include "../../include/Objects/ParticleEmitter.h"

struct ResultRay {
    bool wasHit = false;
    Vertex3D position;
};

class RayLight {
private:
    float intensity;
    float reach;
    float speed;
    float damage;

    Color color;
    Color hit;
    Object3D* parent;

    Vertex3D direction;
    Vertex3D startOffset;

    btCollisionWorld::ClosestRayResultCallback *rayCallback;

    ResultRay result;
public:
    explicit RayLight(bool enabled, Object3D *parent, Vertex3D direction, Vertex3D startOffset, float speed, float damage, Color c, Color hit, int filterGroup, int filterMask);

    void update(bool increase, Vertex3D start);

    void updateDirection(Vertex3D direction, Vertex3D startOffset);

    void setIntensity(float intensity);

    void setDamage(float damage);

    void increaseReach();

    void setColor(const Color &color);

    [[nodiscard]] const Vertex3D &getDirection() const;

    void setDirection(Vertex3D direction);

    void setReach(float reach);

    virtual ~RayLight();

    [[nodiscard]] float getReach() const;

    void drawImGuiProperties();

    void setCollisionMask(int filterGroup, int filterMask);

    ResultRay getResult();

    Object3D *getParent() const;
};


#endif //BRAKEZA3D_RAYLIGHT_H
