//
// Created by eduardo on 8/2/22.
//

#ifndef BRAKEDA3D_PARTICLEEMISSOR_H
#define BRAKEDA3D_PARTICLEEMISSOR_H

#include "Object3D.h"
#include "LightPoint.h"
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
    bool active = true;
    bool stopAdd = false;
    int LastUsedParticle = 0;
    Object3D* followTarget    = nullptr;
    float selfDestructTimer   = -1.0f;

    ParticleEmitterState state;
    ParticlesContext context;

    Image *texture = nullptr;

    LightPoint *attachedLight = nullptr;

    // CPU mode
    Particle ParticlesContainer[MaxParticles];
    GLuint billboard_vertex_buffer   = 0;
    GLuint particles_position_buffer = 0;
    GLuint particles_color_buffer    = 0;

    // GPU mode
    bool   gpuMode    = false;
    float  gpuTotalTime = 0.0f;
    GLuint particles_state_buffer = 0;
    std::vector<OCParticle> gpuParticles;

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

    void onUpdate() override;
    void DrawPropertiesGUI() override;
    void InitGLBuffers();
    int FindUnusedParticle();
    int FindUnusedGPUParticle();
    void SortParticles();
    void Draw();
    void DrawGPU();
    void postUpdate() override;
    glm::vec3 AddNoiseToDirection(const glm::vec3 &direction, int noiseRange);

    void setContext(const ParticlesContext &context);
    void setColorTo(const Color &colorTo);
    void setColorFrom(const Color &colorFrom);
    void setTexture(Image *texture);
    void setStopAdd(bool stopAdd);
    void setGPUMode(bool enabled);

    void detach(float fadeTime);
    void setFollowTarget(Object3D* target)                  { followTarget = target; }
    [[nodiscard]] Object3D* getFollowTarget() const         { return followTarget; }

    void setAttachedLight(LightPoint *light);
    void createAttachedLight(const Color &diffuse, const Color &ambient, const Color &specular, float range);
    void removeAttachedLight();
    [[nodiscard]] LightPoint *getAttachedLight() const      { return attachedLight; }

    [[nodiscard]] bool isActive() const                     { return active; }
    [[nodiscard]] bool isGPUMode() const                    { return gpuMode; }
    [[nodiscard]] ParticlesContext& getContextPointer()     { return context; }
    [[nodiscard]] ObjectType getTypeObject() const override { return ObjectType::ParticleEmitter; }
    GUIType::Sheet getIcon() override                       { return IconObject::PARTICLE_EMITTER; }
    [[nodiscard]] Color getColorTo() const                  { return colorTo; }
    [[nodiscard]] Color getColorFrom() const                { return colorFrom; }
    [[nodiscard]] Image * getTexture() const                { return texture; }

    friend class ParticleEmitterSerializer;
    friend class ParticleEmitterGUI;
};

#endif //BRAKEDA3D_PARTICLEEMISSOR_H
