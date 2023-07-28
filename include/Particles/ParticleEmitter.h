//
// Created by eduardo on 8/2/22.
//

#ifndef BRAKEDA3D_PARTICLEEMISSOR_H
#define BRAKEDA3D_PARTICLEEMISSOR_H


#include <vector>
#include "../Objects/Object3D.h"
#include "../Misc/Timer.h"
#include "../Misc/Counter.h"
#include "../Misc/Color.h"
#include "../../darkheaz/src/shaders/ShaderParticles.h"
#include "../../darkheaz/src/shaders/ShaderExplosion.h"

typedef enum {
    DEFAULT = 0,
    EXPLOSION = 1,
} ParticleEmitterState;

class ParticleEmitter : public Object3D {
private:
    bool active;
    bool stopAdd;
    ParticleEmitterState state;

protected:
    std::vector<OCParticle> oclParticles;

    Counter lifeCounter;
    Color colorTo;
    Color colorFrom;
public:
    ParticleEmitter(
        ParticleEmitterState state,
        Object3D *parent,
        Vertex3D position,
        float ttlEmitter,
        Color colorFrom,
        Color colorTo,
        OCParticlesContext particlesContext
    );

    ~ParticleEmitter() override;

    void onUpdate() override;

    void setActive(bool value);

    [[nodiscard]] bool isActive() const;

    [[nodiscard]] bool isStopAdd() const;

    void setStopAdd(bool stopAdd);

    void drawOnUpdateSecondPass() override;

    ShaderExplosion *shaderExplosion;
    ShaderParticles *shaderParticles;
};


#endif //BRAKEDA3D_PARTICLEEMISSOR_H
