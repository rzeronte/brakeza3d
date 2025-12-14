#ifndef BRAKEZA3D_THREADJOBLOADIMAGE3D_H
#define BRAKEZA3D_THREADJOBLOADIMAGE3D_H

#include "ThreadJobBase.h"
#include "../Brakeza.h"
#include "../3D/Image3D.h"
#include "../Misc/Logging.h"

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
            Logging::Error("[Pools] Image3D pointer is null");
            return;
        }

        Logging::Message("[Pools] ThreadJobLoadImage3D::fnProcess END");
    }

    void fnCallback()
    {
        if (!image) {
            Logging::Error("[Pools] Mesh was destroyed before callback");
            return;
        }

        Image3DSerializer::ApplyImageFromFile(image, json);
        Image3DSerializer::ApplySize(image, json);

        Brakeza::get()->addObject3D(image, image->getName());

        Logging::Message("[ThreadJobLoadImage3D] Callback END");
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