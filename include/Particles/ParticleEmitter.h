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
#include "../Shaders/ShaderParticles.h"
#include "../Shaders/ShaderExplosion.h"

typedef enum {
    DEFAULT = 0,
    EXPLOSION = 1,
} ParticleEmitterState;

class ParticleEmitter : public Object3D {
private:
    bool active;
    bool stopAdd;
    ParticleEmitterState state;
    OCParticlesContext context;
    GLuint particlesBuffer;
protected:
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

    void onUpdate() override;

    [[nodiscard]] bool isActive() const;

    void setStopAdd(bool stopAdd);

    const char *getTypeObject() override;

    const char *getTypeIcon() override;

    void drawImGuiProperties() override;

    cJSON *getJSON() override;

    static void createFromJSON(cJSON *object);

    static void setPropertiesFromJSON(cJSON *object, ParticleEmitter *o);

    void setContext(const OCParticlesContext &context);

    void setColorTo(const Color &colorTo);

    void setColorFrom(const Color &colorFrom);
};


#endif //BRAKEDA3D_PARTICLEEMISSOR_H
