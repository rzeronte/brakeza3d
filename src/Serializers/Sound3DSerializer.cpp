//
// Created by Eduardo on 10/07/2026.
//

#include "../../include/Serializers/Sound3DSerializer.h"
#include "../../include/3D/Sound3D.h"
#include "../../include/Brakeza.h"
#include "../../include/Components/Components.h"
#include "../../include/Misc/ToolsJSON.h"
#include "../../include/Serializers/Object3DSerializer.h"
#include "../../include/Threads/ThreadJobLoadSound3D.h"

cJSON* Sound3DSerializer::JsonByObject(Object3D* o)
{
    auto* s = dynamic_cast<Sound3D*>(o);
    auto* root = Object3DSerializer().JsonByObject(o);

    cJSON_AddStringToObject(root, "sourceFile",   s->sourceFile.c_str());
    cJSON_AddNumberToObject(root, "innerRadius",  s->innerRadius);
    cJSON_AddNumberToObject(root, "outerRadius",  s->outerRadius);
    cJSON_AddNumberToObject(root, "baseVolume",   s->baseVolume);
    cJSON_AddBoolToObject  (root, "loop",         s->loop);
    cJSON_AddBoolToObject  (root, "debugDraw",    s->debugDraw);

    return root;
}

Object3D* Sound3DSerializer::ObjectByJson(cJSON* json)
{
    auto* o = new Sound3D();
    ApplyJsonToObject(json, o);
    Brakeza::get()->PoolCompute().enqueueWithMainThreadCallback(std::make_shared<ThreadJobLoadSound3D>(o, json));
    return o;
}

void Sound3DSerializer::ApplyJsonToObject(cJSON* json, Object3D* base)
{
    auto* s = dynamic_cast<Sound3D*>(base);
    Object3DSerializer().ApplyJsonToObject(json, base);

    auto* sf = cJSON_GetObjectItemCaseSensitive(json, "sourceFile");
    if (sf && sf->valuestring) s->sourceFile = sf->valuestring;

    auto* ir = cJSON_GetObjectItemCaseSensitive(json, "innerRadius");
    if (ir) s->innerRadius = static_cast<float>(ir->valuedouble);

    auto* or_ = cJSON_GetObjectItemCaseSensitive(json, "outerRadius");
    if (or_) s->outerRadius = static_cast<float>(or_->valuedouble);

    auto* bv = cJSON_GetObjectItemCaseSensitive(json, "baseVolume");
    if (bv) s->baseVolume = static_cast<int>(bv->valuedouble);

    auto* lp = cJSON_GetObjectItemCaseSensitive(json, "loop");
    if (lp) s->loop = cJSON_IsTrue(lp);

    auto* dd = cJSON_GetObjectItemCaseSensitive(json, "debugDraw");
    if (dd) s->debugDraw = cJSON_IsTrue(dd);
}

void Sound3DSerializer::MenuLoad(const std::string& model)
{
    auto* o = new Sound3D();
    o->setName(Brakeza::UniqueObjectLabel("Sound3D"));
    o->setPosition(Components::get()->Camera()->getCamera()->getPosition());
    o->sourceFile = model.empty() ? "../assets/sounds/radio_beep.wav" : model;

    auto* json = Sound3DSerializer::JsonByObject(o);
    Brakeza::get()->PoolCompute().enqueueWithMainThreadCallback(std::make_shared<ThreadJobLoadSound3D>(o, json));
}
