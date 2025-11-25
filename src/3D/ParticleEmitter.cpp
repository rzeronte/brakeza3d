#define GL_GLEXT_PROTOTYPES

#include <glm/ext/matrix_transform.hpp>
#include "../../include/3D/ParticleEmitter.h"
#include "../../include/Render/Transforms.h"
#include "../../include/Components/ComponentsManager.h"
#include "../../include/Brakeza.h"
#include "../../include/GUI/Objects/ParticleEmitterGUI.h"

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
    active(true),
    stopAdd(false),
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

    // The VBO containing the 4 vertices of the particles.
    // Thanks to instancing, they will be shared by all particles.
    static const GLfloat g_vertex_buffer_data[] = {
            -0.5f, -0.5f, 0.0f,
            0.5f, -0.5f, 0.0f,
            -0.5f,  0.5f, 0.0f,
            0.5f,  0.5f, 0.0f,
    };

    glGenBuffers(1, &billboard_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    // The VBO containing the positions and sizes of the particles
    glGenBuffers(1, &particles_position_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
    // Initialize with empty (NULL) buffer : it will be updated later, each frame.
    glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);

    // The VBO containing the colors of the particles
    glGenBuffers(1, &particles_color_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
    // Initialize with empty (NULL) buffer : it will be updated later, each frame.
    glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW);

    for (int i=0; i<MaxParticles; i++){
        ParticlesContainer[i].life = -1.0f;
        ParticlesContainer[i].cameradistance = -1.0f;
    }
}

bool ParticleEmitter::isActive() const {
    return active;
}

void ParticleEmitter::postUpdate()
{
    Object3D::postUpdate();
    draw();
}

void ParticleEmitter::onUpdate()
{
    Object3D::onUpdate();

}

void ParticleEmitter::draw()
{
    if (isRemoved() || !isActive() || !isEnabled() || texture == nullptr) return;

    if (parent != nullptr && !parent->isRemoved()) {
        setPosition(parent->getPosition());
    }

    lifeCounter.update();
    if (this->lifeCounter.isFinished()) {
        setRemoved(true);
    }

    Vertex3D direction = getRotation() * Vertex3D(0, 1, 0);

    float delta = Brakeza::get()->getDeltaTime();
    glm::vec3 CameraPosition(ComponentsManager::get()->getComponentCamera()->getCamera()->getPosition().toGLM());

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
            maindir = addNoiseToDirection(maindir, context.SMOKE_ANGLE_RANGE/2);

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
                p.speed = addNoiseToDirection(p.speed, context.VELOCITY_NOISE/2);

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

            }else{
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

    ComponentsManager::get()->getComponentRender()->getShaderOGLParticles()->render(
            billboard_vertex_buffer,
            particles_position_buffer,
            particles_color_buffer,
            texture->getOGLTextureID(),
            ParticlesCount
    );
}

void ParticleEmitter::setStopAdd(bool stopAdd)
{
    ParticleEmitter::stopAdd = stopAdd;
}

const char *ParticleEmitter::getTypeObject()
{
    return SceneObjectTypes::PARTICLE_EMITTER;
}

const char *ParticleEmitter::getTypeIcon()
{
    return IconsByObject::PARTICLE_EMITTER;
}

void ParticleEmitter::drawImGuiProperties()
{
    Object3D::drawImGuiProperties();
    ParticleEmitterGUI::drawImGuiProperties(this);
}

void ParticleEmitter::setContext(const ParticlesContext &context) {
    ParticleEmitter::context = context;
}

void ParticleEmitter::setColorTo(const Color &colorTo) {
    ParticleEmitter::colorTo = colorTo;
}

void ParticleEmitter::setColorFrom(const Color &colorFrom) {
    ParticleEmitter::colorFrom = colorFrom;
}

[[maybe_unused]] void ParticleEmitter::SortParticles(){
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

    for(int i=0; i<LastUsedParticle; i++){
        if (ParticlesContainer[i].life < 0){
            LastUsedParticle = i;
            return i;
        }
    }

    return 0; // All particles are taken, override the first one
}

// Función para agregar ruido a un glm::vec3 que representa una dirección
glm::vec3 ParticleEmitter::addNoiseToDirection(const glm::vec3& direction, int noiseRange) {
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

ParticleEmitter *ParticleEmitter::create(
    const Vertex3D &position,
    float ttl,
    const Color &cf,
    const Color &ct,
    ParticlesContext context,
    const std::string& imageFile
)
{
    auto *o = new ParticleEmitter(
        DEFAULT,
        nullptr,
        position,
        ttl,
        cf,
        ct,
        context,
        new Image(imageFile)
    );

    return o;
}

ParticlesContext& ParticleEmitter::getContextPointer()
{
    return context;
}

Color ParticleEmitter::getColorTo() const
{
    return colorTo;
}

Color ParticleEmitter::getColorFrom() const
{
    return colorFrom;
}

Image * ParticleEmitter::getTexture() const
{
    return texture;
}

void ParticleEmitter::setTexture(Image *texture)
{
    ParticleEmitter::texture = texture;
}
