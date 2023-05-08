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

class ParticleEmitter : public Object3D {
private:
    bool active;
    bool stopAdd;
    float rotFrameX;
    float rotFrameY;
    float rotFrameZ;
    ShaderParticles *shaderParticles;
protected:
    std::vector<OCParticle> oclParticles;

    Counter lifeCounter;
    float force;
    float ttl;
    Color colorTo;
    Color colorFrom;
public:
    ParticleEmitter(
        Object3D *parent,
        Vertex3D position,
        float ttlEmitter,
        float force,
        float ttl,
        float step,
        Color colorFrom,
        Color colorTo,
        Vertex3D rotationFrame
    );

    ~ParticleEmitter() override;

    void setRotationFrame(float, float, float);

    void onUpdate() override;

    void setActive(bool value);

    [[nodiscard]] bool isActive() const;

    bool isStopAdd() const;

    void setStopAdd(bool stopAdd);

};


#endif //BRAKEDA3D_PARTICLEEMISSOR_H
