#define GL_GLEXT_PROTOTYPES

#include <algorithm>
#include <glm/ext/matrix_transform.hpp>
#include "../../include/3D/ParticleEmitter.h"
#include "../../include/Render/Transforms.h"
#include "../../include/Components/Components.h"
#include "../../include/Brakeza.h"
#include "../../include/GUI/Objects/ParticleEmitterGUI.h"
#include "../../include/Misc/Tools.h"

ParticleEmitter::ParticleEmitter(
    ParticleEmitterState state,
    Object3D *parent,
    Vertex3D position,
    float ttlEmitter,
    const Color &colorFrom,
    const Color &colorTo,
    ParticlesContext particlesContext,
    Image *image
) :
    state(state),
    context(particlesContext),
    texture(image),
    lifeCounter(Counter(ttlEmitter)),
    colorTo(colorTo),
    colorFrom(colorFrom)
{
    setParent(parent);
    setPosition(position);

    lifeCounter.setEnabled(true);

    for (int i = 0; i < MaxParticles; i++) {
        ParticlesContainer[i].life = -1.0f;
        ParticlesContainer[i].cameradistance = -1.0f;
    }
    // GL buffers are created lazily in Draw() / DrawGPU() on the GL thread
}

void ParticleEmitter::InitGLBuffers()
{
    static const GLfloat g_vertex_buffer_data[] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
        -0.5f,  0.5f, 0.0f,
         0.5f,  0.5f, 0.0f,
    };

    glGenBuffers(1, &billboard_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    glGenBuffers(1, &particles_position_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
    glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLfloat), nullptr, GL_STREAM_DRAW);

    glGenBuffers(1, &particles_color_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
    glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLubyte), nullptr, GL_STREAM_DRAW);
}

void ParticleEmitter::postUpdate()
{
    Object3D::postUpdate();
    if (gpuMode) {
        DrawGPU();
    } else {
        Draw();
    }
}

void ParticleEmitter::onUpdate()
{
    Object3D::onUpdate();

    if (followTarget != nullptr) {
        if (!followTarget->isRemoved())
            setPosition(followTarget->getPosition());
        else
            followTarget = nullptr;
    }

    if (selfDestructTimer >= 0.0f) {
        selfDestructTimer -= Brakeza::get()->getDeltaTime();
        if (selfDestructTimer <= 0.0f)
            setRemoved(true);
    }

    if (attachedLight != nullptr && !attachedLight->isRemoved()) {
        attachedLight->setPosition(getPosition());
    }
}

void ParticleEmitter::detach(float fadeTime)
{
    followTarget      = nullptr;
    stopAdd           = true;
    selfDestructTimer = fadeTime;
}

void ParticleEmitter::Draw()
{
    if (isRemoved() || !isActive() || !isEnabled() || texture == nullptr) return;

    if (billboard_vertex_buffer == 0) InitGLBuffers();

    if (parent != nullptr && !parent->isRemoved()) {
        setPosition(parent->getPosition());
    }

    lifeCounter.update();
    if (this->lifeCounter.isFinished()) {
        setRemoved(true);
    }

    Vertex3D direction = getRotation() * Vertex3D(0, 1, 0);

    float delta = Brakeza::get()->getDeltaTime();
    glm::vec3 CameraPosition(Components::get()->Camera()->getCamera()->getPosition().toGLM());

    // Define la frecuencia deseada en partículas por segundo
    int newparticles = static_cast<int>(delta * 100 * context.PARTICLES_BY_SECOND);

    int ParticlesCount = 0;

    if (!stopAdd) {
        for (int i = 0; i < newparticles; i++){
            int particleIndex = FindUnusedParticle();
            ParticlesContainer[particleIndex].life = context.PARTICLE_LIFESPAN; // This particle will live 5 seconds.
            ParticlesContainer[particleIndex].pos = getPosition().toGLM();

            ParticlesContainer[particleIndex].pos += glm::vec3(
                    static_cast<float>(Tools::random(-context.POSITION_NOISE / 2, context.POSITION_NOISE / 2)),
                    static_cast<float>(Tools::random(-context.POSITION_NOISE / 2, context.POSITION_NOISE / 2)),
                    static_cast<float>(Tools::random(-context.POSITION_NOISE / 2, context.POSITION_NOISE / 2))
            ) * delta;

            glm::vec3 maindir = direction.toGLM();
            maindir = AddNoiseToDirection(maindir, context.SMOKE_ANGLE_RANGE/2);

            ParticlesContainer[particleIndex].speed = maindir * (Tools::random(context.MIN_VELOCITY, context.MAX_VELOCITY) * delta);

            // Very bad way to generate a random color
            ParticlesContainer[particleIndex].r = rand() % 256;
            ParticlesContainer[particleIndex].g = rand() % 256;
            ParticlesContainer[particleIndex].b = rand() % 256;
            ParticlesContainer[particleIndex].a = Tools::random(context.MIN_ALPHA, context.MAX_ALPHA);

            ParticlesContainer[particleIndex].size = (rand()%1000)/2000.0f + 0.1f;
        }
    }

    static GLfloat* g_particule_position_size_data = new GLfloat[MaxParticles * 4];
    static GLubyte* g_particule_color_data         = new GLubyte[MaxParticles * 4];

    // Simulate all particles
    for (int i = 0; i < MaxParticles; i++){

        Particle& p = ParticlesContainer[i]; // shortcut

        if (p.life > 0.0f) {

            // Decrease life
            p.life -= delta;
            if (p.life > 0.0f){

                // Simulate simple physics : gravity only, no collisions
                p.speed += glm::vec3(0.0f,context.GRAVITY, 0.0f) * delta;

                // Generar factores de ruido para cada componente de glm::vec3
                p.speed = AddNoiseToDirection(p.speed, context.VELOCITY_NOISE/2);

                p.speed *= context.DECELERATION_FACTOR;

                p.pos += p.speed * delta;
                p.cameradistance = glm::length( p.pos - CameraPosition );

                float lifeRatio = p.life / context.PARTICLE_LIFESPAN;
                p.r = colorFrom.r * 255 * lifeRatio + colorTo.r * 255 * (1 - lifeRatio);
                p.g = colorFrom.g * 255 * lifeRatio + colorTo.g * 255 * (1 - lifeRatio);
                p.b = colorFrom.b * 255 * lifeRatio + colorTo.b * 255 * (1 - lifeRatio);

                // Fill the GPU buffer
                g_particule_position_size_data[4*ParticlesCount+0] = p.pos.x;
                g_particule_position_size_data[4*ParticlesCount+1] = p.pos.y;
                g_particule_position_size_data[4*ParticlesCount+2] = p.pos.z;

                g_particule_position_size_data[4*ParticlesCount+3] = p.size;

                g_particule_color_data[4*ParticlesCount+0] = p.r;
                g_particule_color_data[4*ParticlesCount+1] = p.g;
                g_particule_color_data[4*ParticlesCount+2] = p.b;
                g_particule_color_data[4*ParticlesCount+3] = p.a;

            } else {
                // Particles that just died will be put at the end of the buffer in SortParticles();
                p.cameradistance = -1.0f;
            }
            ParticlesCount++;
        }
    }

    SortParticles();

    glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
    glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
    glBufferSubData(GL_ARRAY_BUFFER, 0, ParticlesCount * sizeof(GLfloat) * 4, g_particule_position_size_data);

    glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
    glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
    glBufferSubData(GL_ARRAY_BUFFER, 0, ParticlesCount * sizeof(GLubyte) * 4, g_particule_color_data);

    Components::get()->Render()->getShaders()->shaderOGLParticles->render(
        billboard_vertex_buffer,
        particles_position_buffer,
        particles_color_buffer,
        texture->getOGLTextureID(),
        ParticlesCount
    );
}

void ParticleEmitter::DrawPropertiesGUI()
{
    Object3D::DrawPropertiesGUI();
    ParticleEmitterGUI::DrawPropertiesGUI(this);
}

[[maybe_unused]] void ParticleEmitter::SortParticles()
{
    std::sort(&ParticlesContainer[0], &ParticlesContainer[MaxParticles]);
}

int ParticleEmitter::FindUnusedParticle()
{
    for(int i=LastUsedParticle; i<MaxParticles; i++){
        if (ParticlesContainer[i].life < 0){
            LastUsedParticle = i;
            return i;
        }
    }

    for(int i=0; i<LastUsedParticle; i++) {
        if (ParticlesContainer[i].life < 0) {
            LastUsedParticle = i;
            return i;
        }
    }

    return 0; // All particles are taken, override the first one
}

glm::vec3 ParticleEmitter::AddNoiseToDirection(const glm::vec3& direction, int noiseRange) {
    // Generar ángulos de ruido en los tres ejes
    float noiseX = glm::radians(static_cast<float>(Tools::random(-noiseRange, noiseRange)));
    float noiseY = glm::radians(static_cast<float>(Tools::random(-noiseRange, noiseRange)));
    float noiseZ = glm::radians(static_cast<float>(Tools::random(-noiseRange, noiseRange)));

    // Crear una matriz de rotación a partir de los ángulos de ruido
    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), noiseX, glm::vec3(1.0f, 0.0f, 0.0f));
    rotationMatrix = glm::rotate(rotationMatrix, noiseY, glm::vec3(0.0f, 1.0f, 0.0f));
    rotationMatrix = glm::rotate(rotationMatrix, noiseZ, glm::vec3(0.0f, 0.0f, 1.0f));

    // Aplicar la matriz de rotación al vector de dirección original
    glm::vec3 noisyDirection = glm::vec3(rotationMatrix * glm::vec4(direction, 1.0f));

    return glm::normalize(noisyDirection);  // Normalizar el vector resultante
}

void ParticleEmitter::setTexture(Image *texture)
{
    ParticleEmitter::texture = texture;
}

void ParticleEmitter::setContext(const ParticlesContext &context)
{
    ParticleEmitter::context = context;
}

void ParticleEmitter::setColorTo(const Color &colorTo)
{
    ParticleEmitter::colorTo = colorTo;
}

void ParticleEmitter::setColorFrom(const Color &colorFrom)
{
    ParticleEmitter::colorFrom = colorFrom;
}

void ParticleEmitter::setStopAdd(bool stopAdd)
{
    ParticleEmitter::stopAdd = stopAdd;
}

int ParticleEmitter::FindUnusedGPUParticle()
{
    for (int i = LastUsedParticle; i < NUM_GPU_PARTICLES; i++) {
        if (gpuParticles[i].active < 0.5f) {
            LastUsedParticle = i;
            return i;
        }
    }
    for (int i = 0; i < LastUsedParticle; i++) {
        if (gpuParticles[i].active < 0.5f) {
            LastUsedParticle = i;
            return i;
        }
    }
    return 0;
}

void ParticleEmitter::DrawGPU()
{
    if (isRemoved() || !isActive() || !isEnabled() || texture == nullptr) return;

    if (billboard_vertex_buffer == 0) InitGLBuffers();

    if (gpuParticles.empty()) {
        gpuParticles.resize(NUM_GPU_PARTICLES);
        for (auto &p : gpuParticles) {
            p.active = 0.0f;
        }
    }

    if (particles_state_buffer == 0) {
        glGenBuffers(1, &particles_state_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, particles_state_buffer);
        glBufferData(GL_ARRAY_BUFFER,
            static_cast<GLsizeiptr>(NUM_GPU_PARTICLES * sizeof(OCParticle)),
            nullptr, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    if (parent != nullptr && !parent->isRemoved()) {
        setPosition(parent->getPosition());
    }

    lifeCounter.update();
    if (lifeCounter.isFinished()) {
        setRemoved(true);
        return;
    }

    float delta = Brakeza::get()->getDeltaTime();
    gpuTotalTime += delta;

    Vertex3D direction = getRotation() * Vertex3D(0, 1, 0);
    glm::vec3 dir = direction.toGLM();

    int newParticles = static_cast<int>(delta * 100 * context.PARTICLES_BY_SECOND);

    if (!stopAdd) {
        for (int i = 0; i < newParticles; i++) {
            int idx = FindUnusedGPUParticle();
            OCParticle &p = gpuParticles[idx];

            p.active = 1.0f;
            p.timeToLive = context.PARTICLE_LIFESPAN;
            p.timeLiving = 0.0f;

            p.position = glm::vec4(
                getPosition().toGLM() + glm::vec3(
                    static_cast<float>(Tools::random(-context.POSITION_NOISE / 2, context.POSITION_NOISE / 2)),
                    static_cast<float>(Tools::random(-context.POSITION_NOISE / 2, context.POSITION_NOISE / 2)),
                    static_cast<float>(Tools::random(-context.POSITION_NOISE / 2, context.POSITION_NOISE / 2))
                ) * delta,
                (rand() % 1000) / 2000.0f + 0.1f
            );

            glm::vec3 maindir = AddNoiseToDirection(dir, context.SMOKE_ANGLE_RANGE / 2);
            float speed = Tools::random(context.MIN_VELOCITY, context.MAX_VELOCITY) * delta;
            p.velocity = glm::vec4(maindir * speed, 0.0f);
            p.rotation = glm::vec4(0.0f);
            p.force = Tools::random(context.MIN_ALPHA, context.MAX_ALPHA) / 255.0f;
        }
    }

    int aliveCount = 0;
    for (auto &p : gpuParticles) {
        if (p.active < 0.5f) continue;

        p.timeLiving += delta;

        if (p.timeLiving >= p.timeToLive) {
            p.active = 0.0f;
            continue;
        }

        p.velocity.y += context.GRAVITY * delta;
        glm::vec3 vel = glm::vec3(p.velocity);
        vel = AddNoiseToDirection(vel, context.VELOCITY_NOISE / 2);
        p.velocity = glm::vec4(vel, 0.0f);
        p.velocity *= context.DECELERATION_FACTOR;
        p.position = glm::vec4(glm::vec3(p.position) + glm::vec3(p.velocity) * delta, p.position.w);

        aliveCount++;
    }

    glBindBuffer(GL_ARRAY_BUFFER, particles_state_buffer);
    glBufferData(GL_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(NUM_GPU_PARTICLES * sizeof(OCParticle)),
        gpuParticles.data(),
        GL_DYNAMIC_DRAW);

    auto* shaders = Components::get()->Render()->getShaders();

    shaders->shaderGPUParticles->render(
        particles_state_buffer,
        billboard_vertex_buffer,
        texture->getOGLTextureID(),
        colorFrom,
        colorTo,
        NUM_GPU_PARTICLES,
        static_cast<int>(sizeof(OCParticle))
    );
}

void ParticleEmitter::setAttachedLight(LightPoint *light)
{
    attachedLight = light;
}

void ParticleEmitter::createAttachedLight(const Color &diffuse, const Color &ambient, const Color &specular, float range)
{
    removeAttachedLight();

    float linear = 4.5f / range;
    float quadratic = 75.0f / (range * range);

    auto *light = new LightPoint(
        glm::vec4(ambient.toGLM(), 0),
        glm::vec4(diffuse.toGLM(), 0),
        glm::vec4(specular.toGLM(), 0),
        1.0f,
        linear,
        quadratic
    );

    light->setName(getName() + "_light");
    light->setPosition(getPosition());

    attachedLight = light;
    AttachObject(light);
}

void ParticleEmitter::removeAttachedLight()
{
    if (attachedLight != nullptr) {
        auto it = std::find(attachedObjects.begin(), attachedObjects.end(), attachedLight);
        if (it != attachedObjects.end()) {
            attachedObjects.erase(it);
        }
        delete attachedLight;
        attachedLight = nullptr;
    }
}

void ParticleEmitter::setGPUMode(bool enabled)
{
    if (gpuMode == enabled) return;
    gpuMode = enabled;
    gpuTotalTime = 0.0f;
    gpuParticles.clear();

    if (!enabled && particles_state_buffer != 0) {
        glDeleteBuffers(1, &particles_state_buffer);
        particles_state_buffer = 0;
    }
}