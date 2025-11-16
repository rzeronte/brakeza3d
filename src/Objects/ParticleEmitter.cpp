#define GL_GLEXT_PROTOTYPES

#include <glm/ext/matrix_transform.hpp>
#include "../../include/Objects/ParticleEmitter.h"
#include "../../include/Render/Transforms.h"
#include "../../include/ComponentsManager.h"
#include "../../include/Brakeza3D.h"

ParticleEmitter::ParticleEmitter(
    ParticleEmitterState state,
    Object3D *parent,
    Vertex3D position,
    float ttlEmitter,
    Color colorFrom,
    Color colorTo,
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

    float delta = Brakeza3D::get()->getDeltaTime();
    glm::vec3 CameraPosition(ComponentsManager::get()->getComponentCamera()->getCamera()->getPosition().toGLM());

    // Define la frecuencia deseada en partículas por segundo
    int newparticles = (int) (delta * 100 * context.PARTICLES_BY_SECOND);

    int ParticlesCount = 0;

    if (!stopAdd) {
        for (int i = 0; i < newparticles; i++){
            int particleIndex = FindUnusedParticle();
            ParticlesContainer[particleIndex].life = context.PARTICLE_LIFESPAN; // This particle will live 5 seconds.
            ParticlesContainer[particleIndex].pos = getPosition().toGLM();

            ParticlesContainer[particleIndex].pos += glm::vec3(
                    (float) Tools::random(-context.POSITION_NOISE/2, context.POSITION_NOISE/2),
                    (float) Tools::random(-context.POSITION_NOISE/2, context.POSITION_NOISE/2),
                    (float) Tools::random(-context.POSITION_NOISE/2, context.POSITION_NOISE/2)
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
            (int) texture->getOGLTextureID(),
            ParticlesCount
    );
}

void ParticleEmitter::setStopAdd(bool stopAdd)
{
    ParticleEmitter::stopAdd = stopAdd;
}

const char *ParticleEmitter::getTypeObject() {
    return "ParticleEmitter";
}

const char *ParticleEmitter::getTypeIcon() {
    return "particlesIcon";
}

void ParticleEmitter::drawImGuiProperties()
{
    Object3D::drawImGuiProperties();

    if (ImGui::CollapsingHeader("ParticleEmitter")) {

        if (ImGui::TreeNode("Image")) {
            if (texture != nullptr) {
                ImGui::Image((ImTextureID) texture->getOGLTextureID(),ImVec2(32, 32));
            } else {
                ImGui::Text("Empty texture. Drag one here!");
            }

            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("IMAGE_ITEM")) {
                    Logging::Message("Dropping image (%s) in emitter %s", payload->Data, getLabel().c_str());
                    IM_ASSERT(payload->DataSize == sizeof(int));
                    auto selection = (char*) payload->Data;
                    auto fullPath = EngineSetup::get()->IMAGES_FOLDER + selection;
                    if (texture == nullptr) {
                        texture = new Image(fullPath);
                    } else {
                        delete texture;
                        texture = new Image(fullPath);
                    }
                    Logging::Message("File %s", selection);
                }
                ImGui::EndDragDropTarget();
            }
            ImGui::TreePop();
        }
        ImGui::Separator();

        if (ImGui::TreeNode("Colors")) {
            ImVec4 color = {colorFrom.r, colorFrom.g, colorFrom.b, 1};
            bool changed_color = ImGui::ColorEdit4("Color From##", (float *) &color, ImGuiColorEditFlags_NoOptions);
            if (changed_color) {
                setColorFrom(Color(color.x,color.y,color.z));
            }
            color = {colorTo.r, colorTo.g, colorTo.b, 1};
            changed_color = ImGui::ColorEdit4("Color To##", (float *) &color, ImGuiColorEditFlags_NoOptions);
            if (changed_color) {
                setColorTo(Color(color.x,color.y,color.z));
            }
            ImGui::TreePop();
        }
        ImGui::Separator();
        if (ImGui::TreeNode("ParticlesContext")) {

            ImGui::Checkbox(std::string("stopAdd").c_str(), &stopAdd);

            const float range_sensibility = 0.01f;
            const float range_min = -100;
            const float range_max = 100;

            const int range_min_int = 5;
            const int range_max_int = 255;

            ImGui::DragScalar("Gravity", ImGuiDataType_Float, &context.GRAVITY, range_sensibility ,&range_min, &range_max, "%f", 1.0f);
            ImGui::DragScalar("Particle lifespan", ImGuiDataType_Float, &context.PARTICLE_LIFESPAN, range_sensibility ,&range_min, &range_max, "%f", 1.0f);
            ImGui::DragScalar("Particles/sec", ImGuiDataType_S32, &context.PARTICLES_BY_SECOND, 1.f , &range_min_int, &range_max_int, "%d", 1.0f);
            ImGui::DragScalar("Angle range", ImGuiDataType_S32, &context.SMOKE_ANGLE_RANGE, 1.f ,&range_min_int, &range_max_int, "%d", 1.0f);
            ImGui::DragScalar("Velocity min", ImGuiDataType_S32, &context.MIN_VELOCITY,1.f, &range_min_int, &range_max_int, "%d", 1.0f);
            ImGui::DragScalar("Velocity max", ImGuiDataType_S32, &context.MAX_VELOCITY, 1.f ,&range_min_int, &range_max_int, "%d", 1.0f);
            ImGui::DragScalar("Alpha min", ImGuiDataType_S32, &context.MIN_ALPHA,1.f, &range_min_int, &range_max_int, "%d", 1.0f);
            ImGui::DragScalar("Alpha max", ImGuiDataType_S32, &context.MAX_ALPHA, 1.f ,&range_min_int, &range_max_int, "%d", 1.0f);
            ImGui::DragScalar("Position noise", ImGuiDataType_S32, &context.POSITION_NOISE, 1.f ,&range_min_int, &range_max_int, "%d", 1.0f);
            ImGui::DragScalar("Velocity noise", ImGuiDataType_S32, &context.VELOCITY_NOISE, 1.f ,&range_min_int, &range_max_int, "%d", 1.0f);
            ImGui::DragScalar("Deceleration", ImGuiDataType_Float, &context.DECELERATION_FACTOR, range_sensibility ,&range_min, &range_max, "%f", 1.0f);

            ImGui::TreePop();
        }
    }
}

cJSON * ParticleEmitter::getJSON()
{
    cJSON *root = Object3D::getJSON();

    cJSON *contextParticles = cJSON_CreateObject();
    cJSON_AddNumberToObject(contextParticles, "GRAVITY", context.GRAVITY);
    cJSON_AddNumberToObject(contextParticles, "PARTICLES_BY_SECOND", context.PARTICLES_BY_SECOND);
    cJSON_AddNumberToObject(contextParticles, "PARTICLE_LIFESPAN", context.PARTICLE_LIFESPAN);
    cJSON_AddNumberToObject(contextParticles, "SMOKE_ANGLE_RANGE", context.SMOKE_ANGLE_RANGE);
    cJSON_AddNumberToObject(contextParticles, "MIN_ALPHA", context.MIN_ALPHA);
    cJSON_AddNumberToObject(contextParticles, "MAX_ALPHA", context.MAX_ALPHA);
    cJSON_AddNumberToObject(contextParticles, "MIN_VELOCITY", context.MIN_VELOCITY);
    cJSON_AddNumberToObject(contextParticles, "MAX_VELOCITY", context.MAX_VELOCITY);
    cJSON_AddNumberToObject(contextParticles, "POSITION_NOISE", context.POSITION_NOISE);
    cJSON_AddNumberToObject(contextParticles, "VELOCITY_NOISE", context.VELOCITY_NOISE);
    cJSON_AddNumberToObject(contextParticles, "DECELERATION_FACTOR", context.DECELERATION_FACTOR);
    cJSON_AddItemToObject(root, "context", contextParticles);

    cJSON *colorFromJSON = cJSON_CreateObject();
    cJSON_AddNumberToObject(colorFromJSON, "r", colorFrom.r);
    cJSON_AddNumberToObject(colorFromJSON, "g", colorFrom.g);
    cJSON_AddNumberToObject(colorFromJSON, "b", colorFrom.b);
    cJSON_AddItemToObject(root, "colorFrom", colorFromJSON);

    cJSON *colorToJSON = cJSON_CreateObject();
    cJSON_AddNumberToObject(colorToJSON, "r", colorTo.r);
    cJSON_AddNumberToObject(colorToJSON, "g", colorTo.g);
    cJSON_AddNumberToObject(colorToJSON, "b", colorTo.b);
    cJSON_AddItemToObject(root, "colorTo", colorToJSON);

    if (texture != nullptr) {
        cJSON_AddStringToObject(root, "texture", texture->getFileName().c_str());
    }

    return root;
}

void ParticleEmitter::createFromJSON(cJSON *object)
{
    auto *o = new ParticleEmitter(
        DEFAULT,
        nullptr,
        Vertex3D(0, 0, 0),
        9999,
        Color::red(),
        Color::green(),
        ParticlesContext::defaultParticlesContext(),
        nullptr
    );

    setPropertiesFromJSON(object, o);

    Brakeza3D::get()->addObject3D(o, cJSON_GetObjectItemCaseSensitive(object, "name")->valuestring);
}

void ParticleEmitter::setPropertiesFromJSON(cJSON *object, ParticleEmitter *o)
{
    o->setBelongToScene(true);
    Object3D::setPropertiesFromJSON(object, o);

    auto contextJSON = cJSON_GetObjectItemCaseSensitive(object, "context");
    ParticlesContext context(
        (float) cJSON_GetObjectItemCaseSensitive(contextJSON, "GRAVITY")->valuedouble,
        (float) cJSON_GetObjectItemCaseSensitive(contextJSON, "PARTICLES_BY_SECOND")->valuedouble,
        (float) cJSON_GetObjectItemCaseSensitive(contextJSON, "PARTICLE_LIFESPAN")->valuedouble,
        (int) cJSON_GetObjectItemCaseSensitive(contextJSON, "SMOKE_ANGLE_RANGE")->valueint,
        (int) cJSON_GetObjectItemCaseSensitive(contextJSON, "MIN_VELOCITY")->valueint,
        (int) cJSON_GetObjectItemCaseSensitive(contextJSON, "MAX_VELOCITY")->valueint,
        (int) cJSON_GetObjectItemCaseSensitive(contextJSON, "MIN_ALPHA")->valueint,
        (int) cJSON_GetObjectItemCaseSensitive(contextJSON, "MAX_ALPHA")->valueint,
        (int) cJSON_GetObjectItemCaseSensitive(contextJSON, "POSITION_NOISE")->valueint,
        (int) cJSON_GetObjectItemCaseSensitive(contextJSON, "VELOCITY_NOISE")->valueint,
        (float) cJSON_GetObjectItemCaseSensitive(contextJSON, "DECELERATION_FACTOR")->valuedouble
    );

    o->setContext(context);

    Color colorFrom;
    auto colorFromJSON = cJSON_GetObjectItemCaseSensitive(object, "colorFrom");
    colorFrom.r = (float) cJSON_GetObjectItemCaseSensitive(colorFromJSON, "r")->valuedouble;
    colorFrom.g = (float) cJSON_GetObjectItemCaseSensitive(colorFromJSON, "g")->valuedouble;
    colorFrom.b = (float) cJSON_GetObjectItemCaseSensitive(colorFromJSON, "b")->valuedouble;
    o->setColorFrom(colorFrom);

    Color colorTo;
    auto colorToJSON = cJSON_GetObjectItemCaseSensitive(object, "colorTo");
    colorFrom.r = (float) cJSON_GetObjectItemCaseSensitive(colorToJSON, "r")->valuedouble;
    colorFrom.g = (float) cJSON_GetObjectItemCaseSensitive(colorToJSON, "g")->valuedouble;
    colorFrom.b = (float) cJSON_GetObjectItemCaseSensitive(colorToJSON, "b")->valuedouble;
    o->setColorTo(colorTo);

    if (cJSON_GetObjectItemCaseSensitive(object, "texture")) {
        o->setTexture(new Image(cJSON_GetObjectItemCaseSensitive(object, "texture")->valuestring));
    }
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


int ParticleEmitter::FindUnusedParticle(){

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
    Vertex3D position,
    float ttl,
    Color cf,
    Color ct,
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

void ParticleEmitter::setTexture(Image *texture) {
    ParticleEmitter::texture = texture;
}
