//
// Created by Eduardo on 24/11/2025.
//

#include "../../include/Serializers/ParticleEmmitterSerializer.h"
#include "../../include/Brakeza.h"
#include "../../include/Serializers/JSONSerializerRegistry.h"
#include "../../include/3D/ParticleEmitter.h"
#include "../../include/Components/Components.h"
#include "../../include/Misc/ToolsJSON.h"
#include "../../include/Serializers/Object3DSerializer.h"

cJSON * ParticleEmmitterSerializer::JsonByObject(Object3D *o)
{
    Logging::Message("[ParticleEmitterSerializer] JsonByObject: %d", (int) o->getTypeObject());

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

    if (light->getTexture() != nullptr) {
        cJSON_AddStringToObject(root, "texture", light->getTexture()->getFileName().c_str());
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

    return o;
}

void ParticleEmmitterSerializer::ApplyJsonToObject(const cJSON *json, Object3D *o)
{
    Logging::Message("[ParticleEmmitterSerializer] ApplyJsonToObject: %d", (int) o->getTypeObject());

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
}

void ParticleEmmitterSerializer::LoadFileIntoScene(const std::string &file)
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

    o->setPosition(Components::get()->Camera()->getCamera()->getPosition());

    Brakeza::get()->addObject3D(o, Brakeza::UniqueObjectLabel("ParticleEmitter"));
}
