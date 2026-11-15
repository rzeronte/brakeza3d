/*
 * Brakeza3D - 3D/2D game engine
 * Author:  Eduardo Rodríguez Álvarez
 * GitHub:  https://github.com/rzeronte/brakeza3d
 * Web:     https://brakeza.com
 * Started: 2018-09-30
 * Copyright (C) 2018-2026 Eduardo Rodríguez Álvarez
 * License: GNU General Public License v3.0 (see LICENSE)
 */
//
// Created by Eduardo on 24/11/2025.
//

#include "../../include/Serializers/Image3DSerializer.h"
#include "../../include/3D/Image3D.h"
#include "../../include/Serializers/Object3DSerializer.h"
#include "../../include/Brakeza.h"
#include "../../include/Components/Components.h"
#include "../../include/Misc/Logging.h"
#include "../../include/Threads/ThreadJobLoadImage3D.h"

cJSON * Image3DSerializer::JsonByObject(Object3D *o)
{
    LOG_MESSAGE("[Image3DSerializer] JsonByObject: %d", o->getTypeObject());

    auto image = dynamic_cast<Image3D*>(o);

    auto root = Object3DSerializer().JsonByObject(o);

    cJSON_AddNumberToObject(root, "width", image->width);
    cJSON_AddNumberToObject(root, "height", image->height);
    cJSON_AddStringToObject(root, "image", image->source.c_str());

    return root;
}

Object3D * Image3DSerializer::ObjectByJson(cJSON *json)
{
    auto o = new Image3D(
        Vertex3D(),
        (float) cJSON_GetObjectItemCaseSensitive(json, "width")->valueint,
        (float) cJSON_GetObjectItemCaseSensitive(json, "height")->valueint,
        nullptr
    );

    ApplyJsonToObject(json, o);

    Brakeza::get()->PoolCompute().enqueueWithMainThreadCallback(std::make_shared<ThreadJobLoadImage3D>(o, json));

    return o;
}

void Image3DSerializer::ApplyJsonToObject(cJSON *json, Object3D *o)
{
    LOG_MESSAGE("[Image3DSerializer] ApplyJsonToObject: %d", o->getTypeObject());

    auto image = dynamic_cast<Image3D*>(o);
    image->setSource(cJSON_GetObjectItemCaseSensitive(json, "image")->valuestring);

    Object3DSerializer().ApplyJsonToObject(json, o);
}

void Image3DSerializer::MenuLoad(const std::string &filename)
{
    auto *o = new Image3D(Components::get()->Camera()->getCamera()->getPosition(), 1.0f, 1.0f, filename);
    o->setName(Brakeza::UniqueObjectLabel("Image3D"));

    auto json = Image3DSerializer::JsonByObject(o);
    Brakeza::get()->PoolCompute().enqueueWithMainThreadCallback(std::make_shared<ThreadJobLoadImage3D>(o, json));
}

std::string Image3DSerializer::ExtractFilePath(cJSON *json)
{
    return cJSON_GetObjectItemCaseSensitive(json, "image")->valuestring;
}

void Image3DSerializer::ApplyImageFromFile(Image3D *image, cJSON *json)
{
    image->setImage(new Image(ExtractFilePath(json)));
}

void Image3DSerializer::ApplySize(Image3D *image, cJSON *json)
{
    image->ResetBuffersToSize(
        (float) cJSON_GetObjectItemCaseSensitive(json, "width")->valueint,
        (float) cJSON_GetObjectItemCaseSensitive(json, "height")->valueint
    );
}
