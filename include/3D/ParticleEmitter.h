//
// Created by eduardo on 8/2/22.
//

#ifndef BRAKEDA3D_PARTICLEEMISSOR_H
#define BRAKEDA3D_PARTICLEEMISSOR_H

#include <vector>
#include "Object3D.h"
#include "../Misc/Counter.h"
#include "../Render/Color.h"
#include "../Render/Image.h"
#include "../OpenGL/ShaderOGLParticles.h"
#include "../Render/ParticlesContext.h"

typedef enum {
    DEFAULT = 0,
    EXPLOSION = 1,
} ParticleEmitterState;

class ParticleEmitter : public Object3D
{
    bool active;
    bool stopAdd;
    int LastUsedParticle = 0;

    ParticleEmitterState state;
    ParticlesContext context;

    Image *texture;

    Particle ParticlesContainer[MaxParticles];

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
        const Color &colorFrom,
        const Color &colorTo,
        ParticlesContext particlesContext,
        Image *image
    );

    int FindUnusedParticle();
    const char *getTypeObject() override;
    const char *getTypeIcon() override;
    void onUpdate() override;
    void setStopAdd(bool stopAdd);
    void DrawPropertiesGUI() override;
    void setContext(const ParticlesContext &context);
    void setColorTo(const Color &colorTo);
    void setColorFrom(const Color &colorFrom);
    void SortParticles();
    void draw();
    void setTexture(Image *texture);
    void postUpdate() override;
    glm::vec3 addNoiseToDirection(const glm::vec3 &direction, int noiseRange);
    [[nodiscard]] bool isActive() const;
    [[nodiscard]] ParticlesContext& getContextPointer();
    [[nodiscard]] Color getColorTo() const;
    [[nodiscard]] Color getColorFrom() const;
    [[nodiscard]] Image * getTexture() const;
    static ParticleEmitter* create(
        const Vertex3D &position,
        float ttl,
        const Color &cf,
        const Color &ct,
        ParticlesContext context,
        const std::string& imageFile
    );

    friend class ParticleEmitterSerializer;
    friend class ParticleEmitterGUI;
};

#endif //BRAKEDA3D_PARTICLEEMISSOR_H
