#define GL_GLEXT_PROTOTYPES

#include "../../include/Particles/ParticleEmitter.h"
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
    OCParticlesContext particlesContext
) :
    active(true),
    stopAdd(false),
    state(state),
    lifeCounter(Counter(ttlEmitter)),
    colorTo(colorTo),
    colorFrom(colorFrom),
    context(particlesContext),
    particlesBuffer(0)
{
    setParent(parent);
    setPosition(position);

    lifeCounter.setEnabled(true);

    auto particles = std::vector<OCParticle>(SHADERGL_NUM_PARTICLES, OCParticle{
        glm::vec4(0),
        glm::vec4(0),
        glm::vec4(0),
        0, 0, 0, 0
    });

    glGenBuffers(1, &particlesBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, particlesBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, (int) (particles.size() * sizeof(OCParticle)), &particles[0], GL_STATIC_DRAW);
}

bool ParticleEmitter::isActive() const {
    return active;
}

void ParticleEmitter::onUpdate()
{
    Object3D::onUpdate();

    if (isRemoved() || !isActive()) return;

    if (parent != nullptr && !parent->isRemoved()) {
        setPosition(parent->getPosition());
    }

    lifeCounter.update();
    if (this->lifeCounter.isFinished()) {
        setRemoved(true);
    }

    auto direction = getRotation() * Vertex3D(0, 0, 1);

    ComponentsManager::get()->getComponentWindow()->getShaderCustomOGLParticles()->render(
        (int) particlesBuffer,
        SHADERGL_NUM_PARTICLES,
        getPosition(),
        direction,
        context,
        colorFrom,
        colorTo
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

    if (ImGui::TreeNode("ParticleEmitter")) {
        ImGui::Checkbox(std::string("stopAdd").c_str(), &stopAdd);
        const float range_color_sensibility = 0.01f;
        const float range_col_min = 0;
        const float range_col_max = 1;

        ImGui::Separator();
        if (ImGui::TreeNode("ColorFrom")) {
            ImGui::DragScalar("x", ImGuiDataType_Float, &colorFrom.r, range_color_sensibility,&range_col_min, &range_col_max, "%f", 1.0f);
            ImGui::DragScalar("y", ImGuiDataType_Float, &colorFrom.g, range_color_sensibility,&range_col_min, &range_col_max, "%f", 1.0f);
            ImGui::DragScalar("z", ImGuiDataType_Float, &colorFrom.b, range_color_sensibility,&range_col_min, &range_col_max, "%f", 1.0f);
            ImGui::TreePop();
        }
        ImGui::Separator();

        if (ImGui::TreeNode("ColorTo")) {
            ImGui::DragScalar("x", ImGuiDataType_Float, &colorTo.r, range_color_sensibility,&range_col_min, &range_col_max, "%f", 1.0f);
            ImGui::DragScalar("y", ImGuiDataType_Float, &colorTo.g, range_color_sensibility,&range_col_min, &range_col_max, "%f", 1.0f);
            ImGui::DragScalar("z", ImGuiDataType_Float, &colorTo.b, range_color_sensibility,&range_col_min, &range_col_max, "%f", 1.0f);
            ImGui::TreePop();
        }
        ImGui::Separator();
        if (ImGui::TreeNode("ParticlesContext")) {
            const float range_sensibility = 0.01f;
            const float range_min = -100;
            const float range_max = 100;

            ImGui::DragScalar("Gravity", ImGuiDataType_Float, &context.GRAVITY, range_sensibility ,&range_min, &range_max, "%f", 1.0f);
            ImGui::DragScalar("Step to Add Particle", ImGuiDataType_Float, &context.STEP_ADD_PARTICLE, range_sensibility ,&range_min, &range_max, "%f", 1.0f);
            ImGui::DragScalar("Particle lifespan", ImGuiDataType_Float, &context.PARTICLE_LIFESPAN, range_sensibility ,&range_min, &range_max, "%f", 1.0f);
            ImGui::DragScalar("Angle range", ImGuiDataType_Float, &context.SMOKE_ANGLE_RANGE, range_sensibility ,&range_min, &range_max, "%f", 1.0f);
            ImGui::DragScalar("Min. Velocity", ImGuiDataType_Float, &context.MIN_VELOCITY, range_sensibility ,&range_min, &range_max, "%f", 1.0f);
            ImGui::DragScalar("Max. Velocity", ImGuiDataType_Float, &context.MAX_VELOCITY, range_sensibility ,&range_min, &range_max, "%f", 1.0f);
            ImGui::DragScalar("Alpha min", ImGuiDataType_Float, &context.ALPHA_MIN, range_sensibility ,&range_min, &range_max, "%f", 1.0f);
            ImGui::DragScalar("Alpha max", ImGuiDataType_Float, &context.ALPHA_MAX, range_sensibility ,&range_min, &range_max, "%f", 1.0f);
            ImGui::DragScalar("Position noise", ImGuiDataType_Float, &context.POSITION_NOISE, range_sensibility ,&range_min, &range_max, "%f", 1.0f);
            ImGui::DragScalar("Velocity noise", ImGuiDataType_Float, &context.VELOCITY_NOISE, range_sensibility ,&range_min, &range_max, "%f", 1.0f);
            ImGui::DragScalar("Deceleration", ImGuiDataType_Float, &context.DECELERATION_FACTOR, range_sensibility ,&range_min, &range_max, "%f", 1.0f);

            ImGui::TreePop();
        }
        ImGui::TreePop();
    }
}

cJSON * ParticleEmitter::getJSON()
{
    cJSON *root = Object3D::getJSON();

    cJSON *contextParticles = cJSON_CreateObject();
    cJSON_AddNumberToObject(contextParticles, "GRAVITY", context.GRAVITY);
    cJSON_AddNumberToObject(contextParticles, "STEP_ADD_PARTICLE", context.STEP_ADD_PARTICLE);
    cJSON_AddNumberToObject(contextParticles, "PARTICLE_LIFESPAN", context.PARTICLE_LIFESPAN);
    cJSON_AddNumberToObject(contextParticles, "SMOKE_ANGLE_RANGE", context.SMOKE_ANGLE_RANGE);
    cJSON_AddNumberToObject(contextParticles, "MIN_VELOCITY", context.MIN_VELOCITY);
    cJSON_AddNumberToObject(contextParticles, "MAX_VELOCITY", context.MAX_VELOCITY);
    cJSON_AddNumberToObject(contextParticles, "ALPHA_MIN", context.ALPHA_MIN);
    cJSON_AddNumberToObject(contextParticles, "ALPHA_MAX", context.ALPHA_MAX);
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

    return root;
}

void ParticleEmitter::createFromJSON(cJSON *object)
{
    auto *o = new ParticleEmitter(
        ParticleEmitterState::DEFAULT,
        nullptr,
        Vertex3D(0, 0, 0),
        9999,
        Color::red(),
        Color::green(),
        OCParticlesContext::forExplosion()
    );
    ParticleEmitter::setPropertiesFromJSON(object, o);

    Brakeza3D::get()->addObject3D(o, cJSON_GetObjectItemCaseSensitive(object, "name")->valuestring);
}

void ParticleEmitter::setPropertiesFromJSON(cJSON *object, ParticleEmitter *o)
{
    o->setBelongToScene(true);
    Object3D::setPropertiesFromJSON(object, o);

    OCParticlesContext context;
    auto contextJSON = cJSON_GetObjectItemCaseSensitive(object, "context");
    context.GRAVITY = (float) cJSON_GetObjectItemCaseSensitive(contextJSON, "GRAVITY")->valuedouble;
    context.STEP_ADD_PARTICLE = (float) cJSON_GetObjectItemCaseSensitive(contextJSON, "STEP_ADD_PARTICLE")->valuedouble;
    context.PARTICLE_LIFESPAN = (float) cJSON_GetObjectItemCaseSensitive(contextJSON, "PARTICLE_LIFESPAN")->valuedouble;
    context.SMOKE_ANGLE_RANGE = (float) cJSON_GetObjectItemCaseSensitive(contextJSON, "SMOKE_ANGLE_RANGE")->valuedouble;
    context.MIN_VELOCITY = (float) cJSON_GetObjectItemCaseSensitive(contextJSON, "MIN_VELOCITY")->valuedouble;
    context.MAX_VELOCITY = (float) cJSON_GetObjectItemCaseSensitive(contextJSON, "MAX_VELOCITY")->valuedouble;
    context.ALPHA_MIN = (float) cJSON_GetObjectItemCaseSensitive(contextJSON, "ALPHA_MIN")->valuedouble;
    context.ALPHA_MAX = (float) cJSON_GetObjectItemCaseSensitive(contextJSON, "ALPHA_MAX")->valuedouble;
    context.POSITION_NOISE = (float) cJSON_GetObjectItemCaseSensitive(contextJSON, "POSITION_NOISE")->valuedouble;
    context.VELOCITY_NOISE = (float) cJSON_GetObjectItemCaseSensitive(contextJSON, "VELOCITY_NOISE")->valuedouble;
    context.DECELERATION_FACTOR = (float) cJSON_GetObjectItemCaseSensitive(contextJSON, "DECELERATION_FACTOR")->valuedouble;
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
}

void ParticleEmitter::setContext(const OCParticlesContext &context) {
    ParticleEmitter::context = context;
}

void ParticleEmitter::setColorTo(const Color &colorTo) {
    ParticleEmitter::colorTo = colorTo;
}

void ParticleEmitter::setColorFrom(const Color &colorFrom) {
    ParticleEmitter::colorFrom = colorFrom;
}
