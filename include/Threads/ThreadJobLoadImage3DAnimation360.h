#ifndef BRAKEZA3D_THREADJOBLOADIMAGE3DANIMATION360_H
#define BRAKEZA3D_THREADJOBLOADIMAGE3DANIMATION630_H

#include "ThreadJobBase.h"
#include "../Brakeza.h"
#include "../3D/Image3DAnimation360.h"
#include "../Misc/Logging.h"

class ThreadJobLoadImage3DAnimation360: public ThreadJobBase
{
    Image3DAnimation360 *image = nullptr;
    cJSON *json = nullptr;
public:

    ThreadJobLoadImage3DAnimation360(Image3DAnimation360 *image, cJSON *json)
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
            Logging::Error("[ThreadJobLoadImage3DAnimation360] Pointer is null");
            return;
        }

        Image3DAnimation360Serializer::ApplyCreateDirectionalAnimations(image, json);
        Logging::Message("[ThreadJobLoadImage3DAnimation360] Process END");
    }

    void fnCallback()
    {
        if (!image) {
            Logging::Error("[Pools] Mesh was destroyed before callback");
            return;
        }

        Image3DAnimation360Serializer::LoadAnimationTexturesMainThread(image, json);

        Brakeza::get()->addObject3D(image, image->getName());

        Logging::Message("[ThreadJobLoadImage3DAnimation360] Callback END");
    }

    ~ThreadJobLoadImage3DAnimation360()
    {
        if (json) {
            cJSON_Delete(json);
            json = nullptr;
        }
    }
};

#endif //BRAKEZA3D_THREADJOBLOADIMAGE3DANIMATION360_H