//
// Created by Eduardo on 24/11/2025.
//

#include "../../include/Serializers/ParticleEmmitterSerializer.h"
#include "../../include/Brakeza.h"
#include "../../include/Render/JSONSerializerRegistry.h"
#include "../../include/3D/ParticleEmitter.h"
#include "../../include/Components/Components.h"
#include "../../include/Misc/ToolsJSON.h"
#include "../../include/Serializers/Object3DSerializer.h"
#include "../../include/Threads/ThreadJobLoadParticleEmitter.h"

cJSON * ParticleEmmitterSerializer::JsonByObject(Object3D *o)
{
    LOG_MESSAGE("[ParticleEmitterSerializer] JsonByObject: %d", o->getTypeObject());

    auto light = dynamic_cast<ParticleEmitter*>(o);

    auto root = Object3DSerializer().JsonByObject(o);

    cJSON *contextParticles = cJSON_CreateObject();
    cJSON_AddNumberToObject(contextParticles, "GRAVITY", light->getContextPointer().GRAVITY);
    cJSON_AddNumberToObject(contextParticles, "PARTICLES_BY_SECOND", light->getContextPointer().PARTICLES_BY_SECOND);
    cJSON_AddNumberToObject(contextParticles, "PARTICLE_LIFESPAN", light->getContextPointer().PARTICLE_LIFESPAN);
    cJSON_AddNumberToObject(contextParticles, "SMOKE_ANGLE_RANGE", light->getContextPointer().SMOKE_ANGLE_RANGE);
    cJSON_AddNumberToObject(contextParticles, "MIN_ALPHA", light->getContextPointer().MIN_ALPHA);
    cJSON_AddNumberToObject(contextParticles, "MAX_ALPHA", light->getContextPointer().MAX_ALPHA);
    cJSON_AddNumberToObject(contextParticles, "MIN_VELOCITY", light->getContextPointer().MIN_VELOCITY);
    cJSON_AddNumberToObject(contextParticles, "MAX_VELOCITY", light->getContextPointer().MAX_VELOCITY);
    cJSON_AddNumberToObject(contextParticles, "POSITION_NOISE", light->getContextPointer().POSITION_NOISE);
    cJSON_AddNumberToObject(contextParticles, "VELOCITY_NOISE", light->getContextPointer().VELOCITY_NOISE);
    cJSON_AddNumberToObject(contextParticles, "DECELERATION_FACTOR", light->getContextPointer().DECELERATION_FACTOR);
    cJSON_AddItemToObject(root, "context", contextParticles);

    cJSON_AddItemToObject(root, "colorFrom", ToolsJSON::ColorToJSON(light->getColorFrom()));
    cJSON_AddItemToObject(root, "colorTo", ToolsJSON::ColorToJSON(light->getColorTo()));
    cJSON_AddBoolToObject(root, "gpuMode", light->isGPUMode());

    if (light->getTexture() != nullptr) {
        cJSON_AddStringToObject(root, "texture", light->getTexture()->getFileName().c_str());
    }

    if (light->getAttachedLight() != nullptr) {
        auto *lp = light->getAttachedLight();
        cJSON *attachedLightJson = cJSON_CreateObject();
        cJSON_AddItemToObject(attachedLightJson, "ambient", ToolsJSON::Vertex3DToJSON(Vertex3D::fromGLM(lp->ambient)));
        cJSON_AddItemToObject(attachedLightJson, "diffuse", ToolsJSON::Vertex3DToJSON(Vertex3D::fromGLM(lp->diffuse)));
        cJSON_AddItemToObject(attachedLightJson, "specular", ToolsJSON::Vertex3DToJSON(Vertex3D::fromGLM(lp->specular)));
        cJSON_AddNumberToObject(attachedLightJson, "constant", lp->constant);
        cJSON_AddNumberToObject(attachedLightJson, "linear", lp->linear);
        cJSON_AddNumberToObject(attachedLightJson, "quadratic", lp->quadratic);
        cJSON_AddItemToObject(root, "attachedLight", attachedLightJson);
    }

    return root;
}

Object3D * ParticleEmmitterSerializer::ObjectByJson(cJSON *json)
{
    auto *o = new ParticleEmitter(
            DEFAULT,
            nullptr,
            Components::get()->Camera()->getCamera()->getPosition(),
            9999,
            Color::red(),
            Color::green(),
            ParticlesContext(
            0.0f,
            0.5f,
            1.5f,
            25.0f,
            1,
            10,
            125.0f,
            255.0f,
            1,
            1,
            0.99f
        ),
        nullptr
    );

    ApplyJsonToObject(json, o);

    Brakeza::get()->PoolCompute().enqueueWithMainThreadCallback(
        std::make_shared<ThreadJobLoadParticleEmitter>(o, json)
    );

    return o;
}

void ParticleEmmitterSerializer::ApplyJsonToObject(cJSON *json, Object3D *o)
{
    LOG_MESSAGE("[ParticleEmitterSerializer] ApplyJsonToObject: %d", o->getTypeObject());

    auto emitter = dynamic_cast<ParticleEmitter*>(o);

    Object3DSerializer().ApplyJsonToObject(json, o);

    auto contextJSON = cJSON_GetObjectItemCaseSensitive(json, "context");
    ParticlesContext context(
        static_cast<float>(cJSON_GetObjectItemCaseSensitive(contextJSON, "GRAVITY")->valuedouble),
        static_cast<float>(cJSON_GetObjectItemCaseSensitive(contextJSON, "PARTICLES_BY_SECOND")->valuedouble),
        static_cast<float>(cJSON_GetObjectItemCaseSensitive(contextJSON, "PARTICLE_LIFESPAN")->valuedouble),
        cJSON_GetObjectItemCaseSensitive(contextJSON, "SMOKE_ANGLE_RANGE")->valueint,
        cJSON_GetObjectItemCaseSensitive(contextJSON, "MIN_VELOCITY")->valueint,
        cJSON_GetObjectItemCaseSensitive(contextJSON, "MAX_VELOCITY")->valueint,
        cJSON_GetObjectItemCaseSensitive(contextJSON, "MIN_ALPHA")->valueint,
        cJSON_GetObjectItemCaseSensitive(contextJSON, "MAX_ALPHA")->valueint,
        cJSON_GetObjectItemCaseSensitive(contextJSON, "POSITION_NOISE")->valueint,
        cJSON_GetObjectItemCaseSensitive(contextJSON, "VELOCITY_NOISE")->valueint,
        static_cast<float>(cJSON_GetObjectItemCaseSensitive(contextJSON, "DECELERATION_FACTOR")->valuedouble)
    );

    emitter->setContext(context);

    emitter->setColorFrom(ToolsJSON::getColorByJSON(cJSON_GetObjectItemCaseSensitive(json, "colorFrom")));
    emitter->setColorTo(ToolsJSON::getColorByJSON(cJSON_GetObjectItemCaseSensitive(json, "colorTo")));

    if (cJSON_GetObjectItemCaseSensitive(json, "texture")) {
        emitter->setTexture(new Image(cJSON_GetObjectItemCaseSensitive(json, "texture")->valuestring));
    }

    auto gpuModeItem = cJSON_GetObjectItemCaseSensitive(json, "gpuMode");
    if (gpuModeItem && cJSON_IsBool(gpuModeItem)) {
        emitter->setGPUMode(cJSON_IsTrue(gpuModeItem));
    }

    auto attachedLightJson = cJSON_GetObjectItemCaseSensitive(json, "attachedLight");
    if (attachedLightJson) {
        Vertex3D ambient = ToolsJSON::getVertex3DByJSON(cJSON_GetObjectItemCaseSensitive(attachedLightJson, "ambient"));
        Vertex3D diffuse = ToolsJSON::getVertex3DByJSON(cJSON_GetObjectItemCaseSensitive(attachedLightJson, "diffuse"));
        Vertex3D specular = ToolsJSON::getVertex3DByJSON(cJSON_GetObjectItemCaseSensitive(attachedLightJson, "specular"));

        float constant = static_cast<float>(cJSON_GetObjectItemCaseSensitive(attachedLightJson, "constant")->valuedouble);
        float linear = static_cast<float>(cJSON_GetObjectItemCaseSensitive(attachedLightJson, "linear")->valuedouble);
        float quadratic = static_cast<float>(cJSON_GetObjectItemCaseSensitive(attachedLightJson, "quadratic")->valuedouble);

        auto *light = new LightPoint(
            glm::vec4(ambient.toGLM(), 0),
            glm::vec4(diffuse.toGLM(), 0),
            glm::vec4(specular.toGLM(), 0),
            constant, linear, quadratic
        );
        light->setName(emitter->getName() + "_light");
        light->setPosition(emitter->getPosition());
        emitter->AttachObject(light);
        emitter->setAttachedLight(light);
    }
}

void ParticleEmmitterSerializer::MenuLoad(const std::string &file)
{
    auto *o = new ParticleEmitter(
            DEFAULT,
            nullptr,
            Components::get()->Camera()->getCamera()->getPosition(),
            9999,
            Color::red(),
            Color::green(),
            ParticlesContext(
            0.0f,
            0.5f,
            1.5f,
            25.0f,
            1,
            10,
            125.0f,
            255.0f,
            1,
            1,
            0.99f
        ),
        nullptr
    );

    o->setName(Brakeza::UniqueObjectLabel("ParticleEmitter"));
    o->setPosition(Components::get()->Camera()->getCamera()->getPosition());

    auto json = ParticleEmmitterSerializer::JsonByObject(o);
    Brakeza::get()->PoolCompute().enqueueWithMainThreadCallback(std::make_shared<ThreadJobLoadParticleEmitter>(o, json));
}
