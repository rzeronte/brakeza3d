#define GL_GLEXT_PROTOTYPES

#include <algorithm>
#include <cmath>
#include <glm/ext/matrix_transform.hpp>
#include <glm/geometric.hpp>
#include "../../include/3D/ParticleEmitter.h"
#include "../../include/3D/Mesh3D.h"
#include "../../include/Render/Transforms.h"
#include "../../include/Components/Components.h"
#include "../../include/Brakeza.h"
#include "../../include/GUI/Inspectors/ParticleEmitterGUI.h"
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

// ── Espirales / vórtice (ParticlesContext::SWIRL_* / SPIRAL_*) ────────────────────────────────
// Rodrigues: rota v alrededor de `axis` (unitario) `angle` radianes
static glm::vec3 RotateAroundAxis(const glm::vec3 &v, const glm::vec3 &axis, float angle)
{
    const float c = std::cos(angle), s = std::sin(angle);
    return v * c + glm::cross(axis, v) * s + axis * glm::dot(axis, v) * (1.0f - c);
}

static bool HasVortex(const ParticlesContext &c)
{
    return c.SWIRL_SPEED != 0.0f || c.SWIRL_PULL != 0.0f;
}

// Un paso del vórtice: gira posición y velocidad alrededor del eje que pasa por `origin` y atrae hacia él
static void ApplyVortex(glm::vec3 &pos, glm::vec3 &vel, const glm::vec3 &origin, const glm::vec3 &axis,
                        const ParticlesContext &c, float delta)
{
    if (c.SWIRL_SPEED != 0.0f) {
        const float angle = glm::radians(c.SWIRL_SPEED) * delta;
        pos = origin + RotateAroundAxis(pos - origin, axis, angle);
        vel = RotateAroundAxis(vel, axis, angle);
    }
    if (c.SWIRL_PULL != 0.0f) {
        const glm::vec3 rel = pos - origin;
        const glm::vec3 radial = rel - axis * glm::dot(rel, axis);
        vel -= radial * (c.SWIRL_PULL * delta);
    }
}

// Velocidad extra de salida hacia fuera del eje (SPIRAL_RADIAL_SPEED) en el acimut de este nacimiento.
// `phase` avanza SPIRAL_SPIN por partícula; con SPIRAL_ARMS brazos se reparten por igual, sin ellos es aleatorio.
glm::vec3 ParticleEmitter::SpiralLaunchVelocity(const glm::vec3 &axis)
{
    if (context.SPIRAL_RADIAL_SPEED == 0.0f) return glm::vec3(0.0f);

    // Base ortonormal del plano perpendicular al eje
    const glm::vec3 helper = std::fabs(axis.y) < 0.99f ? glm::vec3(0, 1, 0) : glm::vec3(1, 0, 0);
    const glm::vec3 u = glm::normalize(glm::cross(helper, axis));
    const glm::vec3 v = glm::cross(axis, u);

    float azimuth = spiralPhase;
    if (context.SPIRAL_ARMS > 0) {
        azimuth += 6.2831853f * static_cast<float>(spiralArmCounter % context.SPIRAL_ARMS) / static_cast<float>(context.SPIRAL_ARMS);
        spiralArmCounter++;
    } else {
        azimuth += RandomRange(0.0f, 6.2831853f);
    }
    return (u * std::cos(azimuth) + v * std::sin(azimuth)) * context.SPIRAL_RADIAL_SPEED;
}

// ── Formas de emisión (ParticlesContext::SHAPE) ───────────────────────────────────────────────
static glm::vec3 RandomUnitVector()
{
    const float z = RandomRange(-1.0f, 1.0f);
    const float a = RandomRange(0.0f, 6.2831853f);
    const float r = std::sqrt(std::max(0.0f, 1.0f - z * z));
    return glm::vec3(r * std::cos(a), z, r * std::sin(a));
}

static glm::vec3 SafeNormalize(const glm::vec3 &v, const glm::vec3 &fallback)
{
    const float l = glm::length(v);
    return l > 1e-6f ? v / l : fallback;
}

// Punto de la forma en el espacio LOCAL del emisor (eje Y = su "arriba") y normal hacia fuera en ese punto
static void SampleLocalShape(const ParticlesContext &c, glm::vec3 &p, glm::vec3 &n)
{
    const float TWO_PI = 6.2831853f;
    const bool surface = c.SHAPE_SURFACE_ONLY;
    const glm::vec3 up(0.0f, 1.0f, 0.0f);
    p = glm::vec3(0.0f);
    n = up;

    switch (c.SHAPE) {
        case EmitterShape::BOX: {
            const glm::vec3 h(std::max(0.0f, c.SHAPE_SIZE_X) * 0.5f,
                              std::max(0.0f, c.SHAPE_SIZE_Y) * 0.5f,
                              std::max(0.0f, c.SHAPE_SIZE_Z) * 0.5f);
            if (surface) {
                // Cara elegida en proporción a su área; punto uniforme dentro de la cara
                const float ax = h.y * h.z, ay = h.x * h.z, az = h.x * h.y;
                const float pick = RandomRange(0.0f, std::max(1e-9f, ax + ay + az));
                const float s = RandomRange(-1.0f, 1.0f), t = RandomRange(-1.0f, 1.0f);
                const float sign = (rand() & 1) ? 1.0f : -1.0f;
                if (pick < ax)            { p = glm::vec3(sign * h.x, s * h.y, t * h.z); n = glm::vec3(sign, 0, 0); }
                else if (pick < ax + ay)  { p = glm::vec3(s * h.x, sign * h.y, t * h.z); n = glm::vec3(0, sign, 0); }
                else                      { p = glm::vec3(s * h.x, t * h.y, sign * h.z); n = glm::vec3(0, 0, sign); }
            } else {
                p = glm::vec3(RandomRange(-h.x, h.x), RandomRange(-h.y, h.y), RandomRange(-h.z, h.z));
                n = SafeNormalize(p, RandomUnitVector());
            }
            break;
        }
        case EmitterShape::SPHERE: {
            const float r = std::max(0.0f, c.SHAPE_RADIUS);
            const glm::vec3 u = RandomUnitVector();
            const float d = surface ? r : r * std::cbrt(RandomRange(0.0f, 1.0f));
            p = u * d;
            n = u;
            break;
        }
        case EmitterShape::CONE: {
            // Vértice en el emisor; se abre hacia +Y hasta SHAPE_RADIUS a SHAPE_HEIGHT de altura
            const float r = std::max(0.0f, c.SHAPE_RADIUS), h = std::max(0.0001f, c.SHAPE_HEIGHT);
            const float phi = RandomRange(0.0f, TWO_PI);
            const glm::vec3 radial(std::cos(phi), 0.0f, std::sin(phi));
            float t, rho;
            if (surface) { t = std::sqrt(RandomRange(0.0f, 1.0f)); rho = r * t; }              // área lateral ∝ t
            else         { t = std::cbrt(RandomRange(0.0f, 1.0f)); rho = r * t * std::sqrt(RandomRange(0.0f, 1.0f)); }
            p = radial * rho + up * (h * t);
            n = SafeNormalize(radial * h - up * r, up);
            break;
        }
        case EmitterShape::CYLINDER: {
            const float r = std::max(0.0f, c.SHAPE_RADIUS), h = std::max(0.0f, c.SHAPE_HEIGHT);
            const float phi = RandomRange(0.0f, TWO_PI);
            const glm::vec3 radial(std::cos(phi), 0.0f, std::sin(phi));
            if (surface) {
                const float side = TWO_PI * r * h, cap = 3.14159265f * r * r;
                const float pick = RandomRange(0.0f, std::max(1e-9f, side + 2.0f * cap));
                if (pick < side) {
                    p = radial * r + up * RandomRange(-h * 0.5f, h * 0.5f);
                    n = radial;
                } else {
                    const float sign = (pick < side + cap) ? 1.0f : -1.0f;
                    p = radial * (r * std::sqrt(RandomRange(0.0f, 1.0f))) + up * (sign * h * 0.5f);
                    n = up * sign;
                }
            } else {
                p = radial * (r * std::sqrt(RandomRange(0.0f, 1.0f))) + up * RandomRange(-h * 0.5f, h * 0.5f);
                n = radial;
            }
            break;
        }
        case EmitterShape::TORUS: {
            const float R = std::max(0.0f, c.SHAPE_RADIUS), r = std::max(0.0f, c.SHAPE_TUBE_RADIUS);
            const float theta = RandomRange(0.0f, TWO_PI);
            const glm::vec3 ring(std::cos(theta), 0.0f, std::sin(theta));
            float psi = 0.0f, rho = r;
            // Rechazo: el anillo es más largo por fuera que por dentro; sin él, la densidad sería mayor en el interior
            for (int attempt = 0; attempt < 8; attempt++) {
                psi = RandomRange(0.0f, TWO_PI);
                rho = surface ? r : r * std::sqrt(RandomRange(0.0f, 1.0f));
                if (RandomRange(0.0f, 1.0f) <= (R + rho * std::cos(psi)) / std::max(1e-6f, R + r)) break;
            }
            p = ring * (R + rho * std::cos(psi)) + up * (rho * std::sin(psi));
            n = ring * std::cos(psi) + up * std::sin(psi);
            break;
        }
        default:   // POINT (y MESH, que se muestrea aparte): sin volumen
            n = RandomUnitVector();
            break;
    }
}

// Malla de la escena de la que nacen las partículas: se resuelve por nombre cada frame (si se borra o se
// mueve, el emisor lo nota). La tabla de áreas por triángulo se rehace solo al cambiar de malla.
void ParticleEmitter::PrepareShapeMesh()
{
    Mesh3D *mesh = nullptr;
    if (!context.SHAPE_MESH.empty()) {
        Object3D *o = Brakeza::get()->getObjectByName(context.SHAPE_MESH);
        if (o != nullptr && o != this && !o->isRemoved()) mesh = dynamic_cast<Mesh3D *>(o);
    }

    if (mesh == nullptr) {
        shapeMesh = nullptr;
        shapeTriCumArea.clear();
        shapeTris.clear();
        return;
    }

    shapeMeshModel = mesh->getModelMatrix();
    if (mesh == shapeMesh && !shapeTriCumArea.empty()) return;

    shapeMesh = mesh;
    shapeTriCumArea.clear();
    shapeTris.clear();

    float total = 0.0f;
    const auto &data = mesh->getMeshData();
    for (size_t mi = 0; mi < data.size(); mi++) {
        const auto &v = data[mi].vertices;   // triángulos expandidos: 3 vértices consecutivos por triángulo
        for (size_t i = 0; i + 2 < v.size(); i += 3) {
            const glm::vec3 a(shapeMeshModel * v[i]), b(shapeMeshModel * v[i + 1]), c(shapeMeshModel * v[i + 2]);
            const float area = 0.5f * glm::length(glm::cross(b - a, c - a));
            if (area <= 1e-9f) continue;
            total += area;
            shapeTriCumArea.push_back(total);
            shapeTris.emplace_back(static_cast<int>(mi), static_cast<int>(i));
        }
    }
}

bool ParticleEmitter::SampleMeshSurface(glm::vec3 &offset, glm::vec3 &outward)
{
    if (shapeMesh == nullptr || shapeTriCumArea.empty()) return false;

    const float pick = RandomRange(0.0f, shapeTriCumArea.back());
    size_t idx = static_cast<size_t>(std::lower_bound(shapeTriCumArea.begin(), shapeTriCumArea.end(), pick) - shapeTriCumArea.begin());
    if (idx >= shapeTris.size()) idx = shapeTris.size() - 1;

    const auto &data = shapeMesh->getMeshData();
    const int mi = shapeTris[idx].first;
    const size_t vi = static_cast<size_t>(shapeTris[idx].second);
    if (mi < 0 || static_cast<size_t>(mi) >= data.size() || vi + 2 >= data[mi].vertices.size()) return false;   // malla recargada
    const auto &md = data[mi];

    // Punto uniforme dentro del triángulo (coordenadas baricéntricas)
    const float r1 = std::sqrt(RandomRange(0.0f, 1.0f)), r2 = RandomRange(0.0f, 1.0f);
    const float wa = 1.0f - r1, wb = r1 * (1.0f - r2), wc = r1 * r2;

    const glm::vec3 a(shapeMeshModel * md.vertices[vi]), b(shapeMeshModel * md.vertices[vi + 1]), c(shapeMeshModel * md.vertices[vi + 2]);
    const glm::vec3 world = a * wa + b * wb + c * wc;

    glm::vec3 n;
    if (md.normals.size() == md.vertices.size()) {
        n = md.normals[vi] * wa + md.normals[vi + 1] * wb + md.normals[vi + 2] * wc;
        n = glm::mat3(shapeMeshModel) * n;
    } else {
        n = glm::cross(b - a, c - a);
    }

    offset = world - getPosition().toGLM();
    outward = SafeNormalize(n, glm::vec3(0.0f, 1.0f, 0.0f));
    return true;
}

bool ParticleEmitter::SampleShape(glm::vec3 &offset, glm::vec3 &outward)
{
    if (context.SHAPE == EmitterShape::MESH) return SampleMeshSurface(offset, outward);

    glm::vec3 p, n;
    SampleLocalShape(context, p, n);
    // De local a mundo: la forma gira con el emisor
    offset = (getRotation() * Vertex3D(p.x, p.y, p.z)).toGLM();
    outward = SafeNormalize((getRotation() * Vertex3D(n.x, n.y, n.z)).toGLM(), glm::vec3(0.0f, 1.0f, 0.0f));
    return true;
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
    const glm::vec3 axis = glm::normalize(direction.toGLM());       // eje del vórtice / espiral
    const glm::vec3 emitterOrigin = getPosition().toGLM();

    float delta = Brakeza::get()->getDeltaTime();
    glm::vec3 CameraPosition(Components::get()->Camera()->getCamera()->getPosition().toGLM());

    int newparticles = ParticlesToSpawn(delta);
    // VELOCITY_NOISE: grados por frame a 60 fps (±valor/2 por eje), escalado al delta real
    const float velocityNoise = context.VELOCITY_NOISE * 0.5f * delta * 60.0f;
    // DECELERATION_FACTOR está pensado "por frame a 60 fps": a otros fps se ajusta el exponente para
    // que frene lo mismo por segundo
    const float deceleration = std::pow(std::max(0.0f, context.DECELERATION_FACTOR), delta * 60.0f);

    int ParticlesCount = 0;

    // Forma de emisión: con POINT + eje no hay nada que muestrear (comportamiento original)
    const bool useShape = context.SHAPE != EmitterShape::POINT || context.SHAPE_DIRECTION == EmitterShapeDirection::OUTWARD;
    if (useShape && context.SHAPE == EmitterShape::MESH) PrepareShapeMesh();

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
            if (useShape) {
                glm::vec3 shapeOffset, shapeOutward;
                if (SampleShape(shapeOffset, shapeOutward)) {
                    ParticlesContainer[particleIndex].pos += shapeOffset;
                    if (context.SHAPE_DIRECTION == EmitterShapeDirection::OUTWARD) maindir = shapeOutward;
                }
            }
            maindir = AddNoiseToDirection(maindir, context.SMOKE_ANGLE_RANGE/2);

            // Velocidad de salida en u/s
            ParticlesContainer[particleIndex].speed = maindir * RandomRange(
                    static_cast<float>(context.MIN_VELOCITY), static_cast<float>(context.MAX_VELOCITY))
                    + SpiralLaunchVelocity(axis);
            spiralPhase += glm::radians(context.SPIRAL_SPIN) * delta / static_cast<float>(newparticles);

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
                if (HasVortex(context)) ApplyVortex(p.pos, p.speed, emitterOrigin, axis, context, delta);
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
    const glm::vec3 axis = glm::normalize(dir);
    const glm::vec3 emitterOrigin = getPosition().toGLM();

    int newParticles = ParticlesToSpawn(delta);
    const float velocityNoise = context.VELOCITY_NOISE * 0.5f * delta * 60.0f;
    const float deceleration = std::pow(std::max(0.0f, context.DECELERATION_FACTOR), delta * 60.0f);

    const bool useShape = context.SHAPE != EmitterShape::POINT || context.SHAPE_DIRECTION == EmitterShapeDirection::OUTWARD;
    if (useShape && context.SHAPE == EmitterShape::MESH) PrepareShapeMesh();

    if (!stopAdd) {
        for (int i = 0; i < newParticles; i++) {
            int idx = FindUnusedGPUParticle();
            OCParticle &p = gpuParticles[idx];

            p.active = 1.0f;
            p.timeToLive = context.PARTICLE_LIFESPAN;
            p.timeLiving = 0.0f;

            const float halfNoise = static_cast<float>(context.POSITION_NOISE) * 0.5f;
            glm::vec3 spawnPos = getPosition().toGLM() + glm::vec3(
                RandomRange(-halfNoise, halfNoise),
                RandomRange(-halfNoise, halfNoise),
                RandomRange(-halfNoise, halfNoise)
            );
            glm::vec3 emitDir = dir;
            if (useShape) {
                glm::vec3 shapeOffset, shapeOutward;
                if (SampleShape(shapeOffset, shapeOutward)) {
                    spawnPos += shapeOffset;
                    if (context.SHAPE_DIRECTION == EmitterShapeDirection::OUTWARD) emitDir = shapeOutward;
                }
            }
            p.position = glm::vec4(
                spawnPos,
                ((rand() % 1000) / 2000.0f + 0.1f) * context.PARTICLE_SIZE_SCALE   // mismo tamaño que en CPU
            );

            glm::vec3 maindir = AddNoiseToDirection(emitDir, context.SMOKE_ANGLE_RANGE / 2);
            float speed = RandomRange(static_cast<float>(context.MIN_VELOCITY), static_cast<float>(context.MAX_VELOCITY));
            p.velocity = glm::vec4(maindir * speed + SpiralLaunchVelocity(axis), 0.0f);
            spiralPhase += glm::radians(context.SPIRAL_SPIN) * delta / static_cast<float>(newParticles);
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
        if (HasVortex(context)) {
            glm::vec3 pos3 = glm::vec3(p.position);
            ApplyVortex(pos3, vel, emitterOrigin, axis, context, delta);
            p.position = glm::vec4(pos3, p.position.w);
        }
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