#ifndef BRAKEZA3D_THREADJOBLOADIMAGE3DANIMATION_H
#define BRAKEZA3D_THREADJOBLOADIMAGE3DANIMATION_H

#include "ThreadJobBase.h"
#include "../Brakeza.h"
#include "../3D/Image3DAnimation.h"
#include "../Misc/Logging.h"
#include "../Serializers/Image3DAnimationSerializer.h"

class ThreadJobLoadImage3DAnimation: public ThreadJobBase
{
    Image3DAnimation *image = nullptr;
    cJSON *json = nullptr;
public:

    ThreadJobLoadImage3DAnimation(Image3DAnimation *image, cJSON *json)
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

        Image3DAnimationSerializer::ApplyAnimationsBackground(image, json);

        LOG_MESSAGE("[ThreadJobLoadImage3DAnimation] Process END");
    }

    void fnCallback()
    {
        if (!image) {
            LOG_ERROR("[Pools] Mesh was destroyed before callback");
            return;
        }

        Image3DAnimationSerializer::ApplyAnimationsMainThread(image);

        Brakeza::get()->AddObject3D(image, image->getName());

        LOG_MESSAGE("[ThreadJobLoadImage3DAnimation] Callback END");
    }

    ~ThreadJobLoadImage3DAnimation()
    {
        if (json) {
            cJSON_Delete(json);
            json = nullptr;
        }
    }
};

#endif //BRAKEZA3D_THREADJOBLOADIMAGE3DANIMATION_H