#ifndef BRAKEZA3D_THREADJOBLOADIMAGE3D_H
#define BRAKEZA3D_THREADJOBLOADIMAGE3D_H

#include "ThreadJobBase.h"
#include "../Brakeza.h"
#include "../3D/Image3D.h"
#include "../Misc/Logging.h"
#include "../Serializers/Image3DSerializer.h"

class ThreadJobLoadImage3D: public ThreadJobBase
{
    Image3D *image = nullptr;
    cJSON *json = nullptr;
public:

    ThreadJobLoadImage3D(Image3D *image, cJSON *json)
    :
        image(image),
        json(cJSON_Duplicate(json, 1))
    {
        function = [this](){ fnProcess(); };
        callback = [this](){ fnCallback(); };
    }

    void fnProcess()
    {
        if (!image) {
            LOG_ERROR("[Pools] Image3D pointer is null");
            return;
        }

        LOG_MESSAGE("[Pools] ThreadJobLoadImage3D::fnProcess END");
    }

    void fnCallback()
    {
        if (!image) {
            LOG_ERROR("[Pools] Mesh was destroyed before callback");
            return;
        }

        Image3DSerializer::ApplyImageFromFile(image, json);
        Image3DSerializer::ApplySize(image, json);

        Brakeza::get()->AddObject3D(image, image->getName());

        LOG_MESSAGE("[ThreadJobLoadImage3D] Callback END");
    }

    ~ThreadJobLoadImage3D()
    {
        if (json) {
            cJSON_Delete(json);
            json = nullptr;
        }
    }
};

#endif //BRAKEZA3D_THREADJOBLOADIMAGE3D_H