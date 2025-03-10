//
// Created by eduardo on 8/2/22.
//

#ifndef BRAKEDA3D_PARTICLEEMISSOR_H
#define BRAKEDA3D_PARTICLEEMISSOR_H


#include <vector>
#include "Object3D.h"
#include "../Misc/Timer.h"
#include "../Misc/Counter.h"
#include "../Misc/Color.h"
#include "../Misc/Image.h"
#include "../OpenGL/ShaderOpenGLParticles.h"
#include "ParticlesContext.h"

typedef enum {
    DEFAULT = 0,
    EXPLOSION = 1,
} ParticleEmitterState;

class ParticleEmitter : public Object3D {
private:
    bool active;
    bool stopAdd;
    ParticleEmitterState state;
    ParticlesContext context;
    Image *texture;

    Particle ParticlesContainer[MaxParticles];
    int LastUsedParticle = 0;

    GLuint billboard_vertex_buffer;
    GLuint particles_position_buffer;
    GLuint particles_color_buffer;
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
        ParticlesContext particlesContext,
        Image *image
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

    void setContext(const ParticlesContext &context);

    void setColorTo(const Color &colorTo);

    void setColorFrom(const Color &colorFrom);

    void SortParticles();

    int FindUnusedParticle();

    glm::vec3 addNoiseToDirection(const glm::vec3 &direction, int noiseRange);

    void postUpdate() override;

    static ParticleEmitter* create(
        Vertex3D position,
        float ttl,
        Color cf,
        Color ct,
        ParticlesContext context,
        const std::string& imageFile
    );

    void draw();

    void setTexture(Image *texture);
};

#endif //BRAKEDA3D_PARTICLEEMISSOR_H
