#define GL_GLEXT_PROTOTYPES

#include <algorithm>
#include <cmath>
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

    glGenBuffers(1, &particles_rotation_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, particles_rotation_buffer);
    glBufferData(GL_ARRAY_BUFFER, MaxParticles * sizeof(GLfloat), nullptr, GL_STREAM_DRAW);
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

    UpdateFollowAndLifetime();
}

void ParticleEmitter::onUpdateCulled()
{
    Object3D::onUpdateCulled();

    if (isRemoved() || !isEnabled()) return;

    UpdateFollowAndLifetime();

    // El TTL del emisor normalmente se comprueba en Draw()/DrawGPU() (postUpdate), que no corre
    // fuera de cámara: sin esto un emisor con TTL que caduca fuera de pantalla no se elimina nunca.
    lifeCounter.update();
    if (lifeCounter.isFinished()) {
        setRemoved(true);
    }
}

// Seguimiento del objetivo, autodestrucción tras detach() y posición de la luz adjunta. Se llama
// esté o no el emisor en cámara: si solo corriera en onUpdate(), las estelas desenganchadas
// (detach) fuera de pantalla no llegarían nunca a eliminarse y se acumularían sin límite.
void ParticleEmitter::UpdateFollowAndLifetime()
{
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

// Aleatorio uniforme en [a, b] (Tools::random es entero: cuantizaba posiciones y velocidades)
static float RandomRange(float a, float b)
{
    return a + (b - a) * (static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
}

// Rotación de nacimiento de una partícula (ROTATION_*): ángulo inicial y velocidad de giro, radianes
static void RandomParticleRotation(const ParticlesContext& c, float& angle, float& spin)
{
    angle = c.ROTATION_RANDOM_START ? RandomRange(0.0f, 6.2831853f) : 0.0f;
    spin  = glm::radians(RandomRange(c.ROTATION_SPEED_MIN, c.ROTATION_SPEED_MAX));
    if (c.ROTATION_RANDOM_DIR && (rand() & 1)) spin = -spin;
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

    int newparticles = ParticlesToSpawn(delta);
    // VELOCITY_NOISE: grados por frame a 60 fps (±valor/2 por eje), escalado al delta real
    const float velocityNoise = context.VELOCITY_NOISE * 0.5f * delta * 60.0f;
    // DECELERATION_FACTOR está pensado "por frame a 60 fps": a otros fps se ajusta el exponente para
    // que frene lo mismo por segundo
    const float deceleration = std::pow(std::max(0.0f, context.DECELERATION_FACTOR), delta * 60.0f);

    int ParticlesCount = 0;

    if (!stopAdd) {
        for (int i = 0; i < newparticles; i++){
            int particleIndex = FindUnusedParticle();
            ParticlesContainer[particleIndex].life = context.PARTICLE_LIFESPAN; // This particle will live 5 seconds.
            ParticlesContainer[particleIndex].pos = getPosition().toGLM();

            // Reparto del nacimiento en unidades del mundo (±POSITION_NOISE/2 por eje)
            const float halfNoise = static_cast<float>(context.POSITION_NOISE) * 0.5f;
            ParticlesContainer[particleIndex].pos += glm::vec3(
                    RandomRange(-halfNoise, halfNoise),
                    RandomRange(-halfNoise, halfNoise),
                    RandomRange(-halfNoise, halfNoise)
            );

            glm::vec3 maindir = direction.toGLM();
            maindir = AddNoiseToDirection(maindir, context.SMOKE_ANGLE_RANGE/2);

            // Velocidad de salida en u/s
            ParticlesContainer[particleIndex].speed = maindir * RandomRange(
                    static_cast<float>(context.MIN_VELOCITY), static_cast<float>(context.MAX_VELOCITY));

            // Very bad way to generate a random color
            ParticlesContainer[particleIndex].r = rand() % 256;
            ParticlesContainer[particleIndex].g = rand() % 256;
            ParticlesContainer[particleIndex].b = rand() % 256;
            // "weight" (sin otro uso en este struct) guarda el alpha de nacimiento; el alpha
            // real por frame se recalcula en el bucle de simulación de abajo multiplicando por
            // lifeRatio, para que la partícula se desvanezca al morir en vez de desaparecer de
            // golpe (mismo lifeRatio que ya usan r/g/b).
            ParticlesContainer[particleIndex].weight = static_cast<float>(Tools::random(context.MIN_ALPHA, context.MAX_ALPHA));
            ParticlesContainer[particleIndex].a = static_cast<unsigned char>(ParticlesContainer[particleIndex].weight);

            ParticlesContainer[particleIndex].size = ((rand()%1000)/2000.0f + 0.1f) * context.PARTICLE_SIZE_SCALE;
            RandomParticleRotation(context, ParticlesContainer[particleIndex].angle, ParticlesContainer[particleIndex].spin);
        }
    }

    static GLfloat* g_particule_position_size_data = new GLfloat[MaxParticles * 4];
    static GLubyte* g_particule_color_data         = new GLubyte[MaxParticles * 4];
    static GLfloat* g_particule_rotation_data      = new GLfloat[MaxParticles];

    // Simulate all particles
    for (int i = 0; i < MaxParticles; i++){

        Particle& p = ParticlesContainer[i]; // shortcut

        if (p.life > 0.0f) {

            // Decrease life
            p.life -= delta;
            if (p.life > 0.0f){

                // Gravedad (aceleración, u/s²), ruido de dirección que conserva el módulo y frenado
                p.speed += glm::vec3(0.0f,context.GRAVITY, 0.0f) * delta;
                p.speed = AddNoiseKeepingSpeed(p.speed, velocityNoise);
                p.speed *= deceleration;

                p.pos += p.speed * delta;
                p.angle += p.spin * delta;
                p.cameradistance = glm::length( p.pos - CameraPosition );

                float lifeRatio = p.life / context.PARTICLE_LIFESPAN;
                p.r = colorFrom.r * 255 * lifeRatio + colorTo.r * 255 * (1 - lifeRatio);
                p.g = colorFrom.g * 255 * lifeRatio + colorTo.g * 255 * (1 - lifeRatio);
                p.b = colorFrom.b * 255 * lifeRatio + colorTo.b * 255 * (1 - lifeRatio);
                p.a = static_cast<unsigned char>(p.weight * lifeRatio);

                // Fill the GPU buffer
                g_particule_position_size_data[4*ParticlesCount+0] = p.pos.x;
                g_particule_position_size_data[4*ParticlesCount+1] = p.pos.y;
                g_particule_position_size_data[4*ParticlesCount+2] = p.pos.z;

                // Tamaño a lo largo de la vida: SIZE_START al nacer -> SIZE_END al morir
                g_particule_position_size_data[4*ParticlesCount+3] =
                    p.size * (context.SIZE_START * lifeRatio + context.SIZE_END * (1.0f - lifeRatio));

                g_particule_color_data[4*ParticlesCount+0] = p.r;
                g_particule_color_data[4*ParticlesCount+1] = p.g;
                g_particule_color_data[4*ParticlesCount+2] = p.b;
                g_particule_color_data[4*ParticlesCount+3] = p.a;
                g_particule_rotation_data[ParticlesCount] = p.angle;

                ParticlesCount++;
            } else {
                // Particles that just died will be put at the end of the buffer in SortParticles();
                p.cameradistance = -1.0f;
            }
        }
    }

    SortParticles();

    glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
    glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
    glBufferSubData(GL_ARRAY_BUFFER, 0, ParticlesCount * sizeof(GLfloat) * 4, g_particule_position_size_data);

    glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
    glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
    glBufferSubData(GL_ARRAY_BUFFER, 0, ParticlesCount * sizeof(GLubyte) * 4, g_particule_color_data);

    glBindBuffer(GL_ARRAY_BUFFER, particles_rotation_buffer);
    glBufferData(GL_ARRAY_BUFFER, MaxParticles * sizeof(GLfloat), NULL, GL_STREAM_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, ParticlesCount * sizeof(GLfloat), g_particule_rotation_data);

    Components::get()->Render()->getShaders()->shaderOGLParticles->render(
        billboard_vertex_buffer,
        particles_position_buffer,
        particles_color_buffer,
        particles_rotation_buffer,
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

glm::vec3 ParticleEmitter::AddNoiseToDirection(const glm::vec3& direction, float noiseRange) {
    // Generar ángulos de ruido en los tres ejes (decimal: antes era entero y no admitía < 1°)
    float noiseX = glm::radians(RandomRange(-noiseRange, noiseRange));
    float noiseY = glm::radians(RandomRange(-noiseRange, noiseRange));
    float noiseZ = glm::radians(RandomRange(-noiseRange, noiseRange));

    // Crear una matriz de rotación a partir de los ángulos de ruido
    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), noiseX, glm::vec3(1.0f, 0.0f, 0.0f));
    rotationMatrix = glm::rotate(rotationMatrix, noiseY, glm::vec3(0.0f, 1.0f, 0.0f));
    rotationMatrix = glm::rotate(rotationMatrix, noiseZ, glm::vec3(0.0f, 0.0f, 1.0f));

    // Aplicar la matriz de rotación al vector de dirección original
    glm::vec3 noisyDirection = glm::vec3(rotationMatrix * glm::vec4(direction, 1.0f));

    return glm::normalize(noisyDirection);  // Normalizar el vector resultante
}

// Ruido angular sobre una VELOCIDAD: AddNoiseToDirection devuelve un vector unitario, así que hay
// que restaurar el módulo (si no, toda partícula acaba a 1 u/s y MIN/MAX_VELOCITY, la gravedad y
// DECELERATION_FACTOR dejan de tener efecto a partir del segundo frame).
glm::vec3 ParticleEmitter::AddNoiseKeepingSpeed(const glm::vec3 &velocity, float noiseRange)
{
    const float speed = glm::length(velocity);
    if (speed < 1e-6f || noiseRange <= 0.0f) return velocity;
    return AddNoiseToDirection(velocity, noiseRange) * speed;
}

// Partículas a crear este frame (PARTICLES_BY_SECOND por segundo). Acumula la fracción entre frames
// para que ritmos bajos o fps altos no se queden en 0.
int ParticleEmitter::ParticlesToSpawn(float delta)
{
    spawnAccumulator += delta * std::max(0.0f, context.PARTICLES_BY_SECOND);
    const int count = static_cast<int>(spawnAccumulator);
    spawnAccumulator -= static_cast<float>(count);
    return count;
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

    int newParticles = ParticlesToSpawn(delta);
    const float velocityNoise = context.VELOCITY_NOISE * 0.5f * delta * 60.0f;
    const float deceleration = std::pow(std::max(0.0f, context.DECELERATION_FACTOR), delta * 60.0f);

    if (!stopAdd) {
        for (int i = 0; i < newParticles; i++) {
            int idx = FindUnusedGPUParticle();
            OCParticle &p = gpuParticles[idx];

            p.active = 1.0f;
            p.timeToLive = context.PARTICLE_LIFESPAN;
            p.timeLiving = 0.0f;

            const float halfNoise = static_cast<float>(context.POSITION_NOISE) * 0.5f;
            p.position = glm::vec4(
                getPosition().toGLM() + glm::vec3(
                    RandomRange(-halfNoise, halfNoise),
                    RandomRange(-halfNoise, halfNoise),
                    RandomRange(-halfNoise, halfNoise)
                ),
                ((rand() % 1000) / 2000.0f + 0.1f) * context.PARTICLE_SIZE_SCALE   // mismo tamaño que en CPU
            );

            glm::vec3 maindir = AddNoiseToDirection(dir, context.SMOKE_ANGLE_RANGE / 2);
            float speed = RandomRange(static_cast<float>(context.MIN_VELOCITY), static_cast<float>(context.MAX_VELOCITY));
            p.velocity = glm::vec4(maindir * speed, 0.0f);
            float angle, spin;
            RandomParticleRotation(context, angle, spin);
            p.rotation = glm::vec4(angle, spin, 0.0f, 0.0f);   // .x ángulo, .y giro (rad, rad/s)
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
        vel = AddNoiseKeepingSpeed(vel, velocityNoise);
        p.velocity = glm::vec4(vel * deceleration, 0.0f);
        p.position = glm::vec4(glm::vec3(p.position) + glm::vec3(p.velocity) * delta, p.position.w);
        p.rotation.x += p.rotation.y * delta;

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
        context.SIZE_START,
        context.SIZE_END,
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