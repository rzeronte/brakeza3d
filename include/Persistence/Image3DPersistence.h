//
// Created by Eduardo on 23/11/2025.
//

#ifndef BRAKEZA3D_IMAGE3DPERSISTENCE_H
#define BRAKEZA3D_IMAGE3DPERSISTENCE_H

#include "../Objects/Image3D.h"
#include "../Misc/cJSON.h"
#include "../Brakeza3D.h"

inline cJSON *Image3D::getJSON()
{
    auto root= Object3D::getJSON();

    cJSON_AddNumberToObject(root, "width", width);
    cJSON_AddNumberToObject(root, "height", height);
    cJSON_AddStringToObject(root, "image", image->getFileName().c_str());

    return root;
}

inline void Image3D::createFromJSON(cJSON *object)
{
    auto name = cJSON_GetObjectItemCaseSensitive(object, "name")->valuestring;
    auto width = static_cast<float>(cJSON_GetObjectItemCaseSensitive(object, "width")->valueint);
    auto height = static_cast<float>(cJSON_GetObjectItemCaseSensitive(object, "height")->valueint);
    auto image = cJSON_GetObjectItemCaseSensitive(object, "image")->valuestring;

    auto o = new Image3D(
        Vertex3D(),
        width,
        height,
        new Image(image)
    );

    setPropertiesFromJSON(object, o);

    Brakeza3D::get()->addObject3D(o, name);
}

inline void Image3D::setPropertiesFromJSON(cJSON *object, Image3D *o)
{
    o->setBelongToScene(true);

    Object3D::setPropertiesFromJSON(object, o);
}

#endif //BRAKEZA3D_IMAGE3DPERSISTENCE_H