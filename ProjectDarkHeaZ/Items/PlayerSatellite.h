//
// Created by eduardo on 3/07/23.
//

#ifndef BRAKEZA3D_PLAYERSATELLITE_H
#define BRAKEZA3D_PLAYERSATELLITE_H


#include "../../include/Physics/Mesh3DGhost.h"
#include "LivingObject.h"
#include "../../include/Shaders/ShaderParticles.h"

class PlayerSatellite : public Mesh3DGhost, public LivingObject  {
    float damage;
    ShaderParticles *shaderParticles;
public:
    explicit PlayerSatellite(Mesh3D *parent);

    void onUpdate() override;

    void postUpdate() override;

    void onDrawHostBuffer() override;

    void onStartSetup();

    void resolveCollision(Collisionable *collisionable) override;

    void drawOnUpdateSecondPass() override;

    [[nodiscard]] float getDamage() const;

    void updateShaderParticles();
};


#endif //BRAKEZA3D_PLAYERSATELLITE_H